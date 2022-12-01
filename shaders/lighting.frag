#version 330 core

in vec4 position; // raw position in the model coord
in vec3 normal;   // raw normal in the model coord

uniform mat4 modelview; // from model coord to eye coord
uniform mat4 view;      // from world coord to eye coord

// Material parameters
uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform vec4 emision;
uniform float shininess;

// Light source parameters
const int maximal_allowed_lights = 10;
uniform bool enablelighting;
uniform int nlights;
uniform vec4 lightpositions[ maximal_allowed_lights ];
uniform vec4 lightcolors[ maximal_allowed_lights ];

// Output the frag color
out vec4 fragColor;


void main (void){
    if (!enablelighting){
        // Default normal coloring (you don't need to modify anything here)
        vec3 N = normalize(normal);
        fragColor = vec4(0.5f*N + 0.5f , 1.0f);
    } else {
        
        // HW3: You will compute the lighting here.
        
        // transformations computed in the camera coordinate
        //eye position (origin) in camera coord
        vec4 eye = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        
        //position: model -> camera coord
        vec4 P_cam = modelview * position;
        
        //normal: model -> camera coord
        mat3 VM_block = mat3(modelview[0][0],modelview[0][1],modelview[0][2],
                             modelview[1][0],modelview[1][1],modelview[1][2],
                             modelview[2][0],modelview[2][1],modelview[2][2]);
        vec3 N_cam = normalize(inverse(transpose(VM_block)) * normalize(normal));
        
        
        //Blinn-Phong shading equation implementation
        fragColor = emision;
        for (int i=0; i<nlights; i++) {
            //compute L_cam (vector sub: Q_cam - P_cam), Q_cam is light pos.
            vec4 Q_cam = view * lightpositions[i];
            vec3 L_cam = normalize(P_cam.w*Q_cam.xyz - Q_cam.w*P_cam.xyz);;
            
            //compute V_cam (eye - P_cam)
            vec3 V_cam = normalize(P_cam.w*eye.xyz - eye.w*P_cam.xyz);
            
            //compute halfway
            vec3 H_cam = normalize(V_cam + L_cam);

            
            fragColor += (lightcolors[i] *
                          (ambient + diffuse*max(dot(N_cam, L_cam),0) +
                           specular*pow((max(dot(N_cam, H_cam),0)), shininess)
                           ));
        }
    }
}
