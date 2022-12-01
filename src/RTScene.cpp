/**************************************************
Scene.cpp contains the implementation of the draw command
*****************************************************/
#include "RTScene.h"
#include "RTCube.h"
#include "RTObj.h"

// The scene init definition 
#include "RTScene.inl"


using namespace glm;

void RTScene::buildTriangleSoup() {
    // Pre-draw sequence: assign uniforms that are the same for all Geometry::draw call.  These uniforms include the camera view, proj, and the lights.  These uniform do not include modelview and material parameters.
    camera -> computeMatrices();
//    shader -> view = camera -> view;
//    shader -> projection = camera -> proj;
//    shader -> nlights = light.size();
//    shader -> lightpositions.resize( shader -> nlights );
//    shader -> lightcolors.resize( shader -> nlights );
//    int count = 0;
//    for (std::pair<std::string, Light*> entry : light){
//        shader -> lightpositions[ count ] = (entry.second) -> position;
//        shader -> lightcolors[ count ] = (entry.second) -> color;
//        count++;
//    }
    
    // Define stacks for depth-first search (DFS)
    std::stack < RTNode* > dfs_stack;
    std::stack < mat4 >  matrix_stack;
    
    // Initialize the current state variable for DFS
    RTNode* cur = node["world"]; // root of the tree
    mat4 cur_VM = camera -> view;
    
    dfs_stack.push(cur);
    matrix_stack.push(cur_VM);
    
    // Compute total number of connectivities in the graph; this would be an upper bound for
    // the stack size in the depth first search over the directed acyclic graph
    int total_number_of_edges = 0;
    for ( const auto &n : node ) total_number_of_edges += n.second->childnodes.size();
    
    // If you want to print some statistics of your scene graph
     std::cout << "total numb of nodes = " << node.size() << std::endl;
     std::cout << "total number of edges = " << total_number_of_edges << std::endl;
    
    while( ! dfs_stack.empty() ){
        // Detect whether the search runs into infinite loop by checking whether the stack is longer than the number of edges in the graph.
        if ( dfs_stack.size() > total_number_of_edges ){
            std::cerr << "Error: The scene graph has a closed loop." << std::endl;
            exit(-1);
        }
        
        // top-pop the stacks
        cur = dfs_stack.top();  dfs_stack.pop();
        cur_VM = matrix_stack.top();  matrix_stack.pop();
        
        //join triangle lists from all the models at the current node
        for ( size_t i = 0; i < cur -> models.size(); i++ ){
            std::cout << "value of i: " << i << std::endl;
//            // Prepare to draw the geometry. Assign the modelview and the material.
//            shader -> modelview = cur_VM * (cur -> modeltransforms[i]);
//            shader -> material  = ( cur -> models[i] ) -> material;
//
//            // The draw command
//            shader -> setUniforms();
//            ( cur -> models[i] ) -> geometry -> draw();
            
            //add triangles to triangle soup (transformed)
            std::vector<Triangle> triangles = ( cur -> models[i] ) -> geometry -> elements;
//            int count = triangles.size();
            
            std::cout << "Number of triangles for model " << i << ": " << triangles.size() << std::endl;
            
            //TODO: Test! skip shaders!
            //transform all triangles from model coordinate to camera coordinate system
//            for (Triangle tri : triangles) {
//                mat4 tempVM = cur_VM * (cur -> modeltransforms[i]);
//
//                mat3 VM_block = mat3(tempVM[0][0],tempVM[0][1],tempVM[0][2],
//                                     tempVM[1][0],tempVM[1][1],tempVM[1][2],
//                                     tempVM[2][0],tempVM[2][1],tempVM[2][2]);
//                for (size_t j=0; j<3; j++) {
//                    //transform positions
//                    tri.P[j] = VM_block * tri.P[j];
//
//                    //transform normals
//                    tri.N[j] = normalize(inverse(transpose(VM_block)) * normalize(tri.N[j]));
//                }
//
//                //add material to triangle
//                tri.material = ( cur -> models[i] ) -> material;
//
////                std::cout << "Adding triangle" << "..." << std::endl;
//
//                triangle_soup.push_back(tri);
//
//                std::cout << "Added triangle" << "." << std::endl;
//            }
            
            std::cout << "Finished adding triangles for model " << i << std::endl;
        }
        
        // Continue the DFS: put all the child nodes of the current node in the stack
        for ( size_t i = 0; i < cur -> childnodes.size(); i++ ){
            dfs_stack.push( cur -> childnodes[i] );
            matrix_stack.push( cur_VM * (cur -> childtransforms[i]) );
        }
        
    } // End of DFS while loop.
    
    std::cout << "Finished building triangle soup." << std::endl;
}


