/**************************************************
Scene.cpp contains the implementation of the draw command
*****************************************************/
#include "RTScene.h"
#include "RTCube.h"
#include "RTObj.h"

#include <glm/gtx/string_cast.hpp>

// The scene init definition 
#include "RTScene.inl"


using namespace glm;

void RTScene::buildTriangleSoup() {
    // Pre-draw sequence: assign uniforms that are the same for all Geometry::draw call.  These uniforms include the camera view, proj, and the lights.  These uniform do not include modelview and material parameters.
    camera -> computeMatrices();
    
    // reset triangle_soup
    triangle_soup.clear();
    
    // Define stacks for depth-first search (DFS)
    std::stack < RTNode* > dfs_stack;
    std::stack < mat4 >  matrix_stack;
    
    // Initialize the current state variable for DFS
    RTNode* cur = node["world"]; // root of the tree
    mat4 cur_M = mat4(1.0f);
    
    dfs_stack.push(cur);
    matrix_stack.push(cur_M);
    
    // Compute total number of connectivities in the graph; this would be an upper bound for
    // the stack size in the depth first search over the directed acyclic graph
    int total_number_of_edges = 0;

    for ( const auto &n : node ) {
        total_number_of_edges += n.second->childnodes.size();
    }
    
    // If you want to print some statistics of your scene graph
//    std::cout << "total numb of nodes = " << node.size() << std::endl;
//    std::cout << "total number of edges = " << total_number_of_edges << std::endl;
//    std::cout << "triangle_soup size: " << triangle_soup.size() << std::endl;
    
    while( ! dfs_stack.empty() ){
        // Detect whether the search runs into infinite loop by checking whether the stack is longer than the number of edges in the graph.
        if ( dfs_stack.size() > total_number_of_edges ){
            std::cerr << "Error: The scene graph has a closed loop." << std::endl;
            exit(-1);
        }
        
        // top-pop the stacks
        cur = dfs_stack.top();  dfs_stack.pop();
        cur_M = matrix_stack.top();  matrix_stack.pop();
                
        //join triangle lists from all the models at the current node
        for ( size_t i = 0; i < cur -> models.size(); i++ ){
            //add triangles to triangle soup (transformed)
            std::vector<Triangle> triangles = ( cur -> models[i] ) -> geometry -> elements;
                                    
            //TODO: Test! skip shaders!
            //transform all triangles from model coordinate to same coordinate system
            for (Triangle tri : triangles) {
                //in world coordinate system
                mat4 tempMatrix = cur_M * (cur -> modeltransforms[i]);
                mat3 M_block = mat3(tempMatrix[0][0],tempMatrix[0][1],tempMatrix[0][2],
                                     tempMatrix[1][0],tempMatrix[1][1],tempMatrix[1][2],
                                     tempMatrix[2][0],tempMatrix[2][1],tempMatrix[2][2]);

                for (size_t j=0; j<3; j++) {
                    vec4 tempPos = tempMatrix * vec4(tri.P[j], 1.0f);
                    
//                    std::cout << "tempPos[" << j << "]: " << glm::to_string(tempPos) << std::endl;
                    
                    //transform positions
                    tri.P[j] = vec3(tempPos[0]/tempPos[3], tempPos[1]/tempPos[3], tempPos[2]/tempPos[3]);

                    //transform normals
                    tri.N[j] = normalize(inverse(transpose(M_block)) * normalize(tri.N[j]));
                }

                //add material to triangle
                tri.material = ( cur -> models[i] ) -> material;

                triangle_soup.push_back(tri);
            }
            
//            std::cout << "Finished adding triangles for model " << i << std::endl;
        }
        
        // Continue the DFS: put all the child nodes of the current node in the stack
        for ( size_t i = 0; i < cur -> childnodes.size(); i++ ){
            dfs_stack.push( cur -> childnodes[i] );
            matrix_stack.push( cur_M * (cur -> childtransforms[i]) );
        }
        
    } // End of DFS while loop.
    
    std::cout << "Finished building triangle soup." << std::endl;
    std::cout << "triangle_soup size: " << triangle_soup.size() << std::endl;

}


