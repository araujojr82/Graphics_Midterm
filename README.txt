Graphics Midterm 

Author: Euclides Araujo

User input keys:
[LIGHT CONTROL]
1-9: Select lights in the scene;
Backspace: Turn the selected light on/off;
C Key: Change the colour for the selected light;
Arrow Keys: Move the light in X, Y;
[ and ] Keys: Move the light in Z;
,(<): decrease the linear attenuation for selected light;
.(>): increase the linear attenuation for selected light;

[CAMERA CONTROL]
Enter: Turn the wireframe mode on/off;
W,A,S,D,Q,E: Controls the camera position;
I,J,K,L,U,O: Controls the acceleration of the camera;
P: Stop all acceleration


Config is divided in 3 files, the config.txt has the interface options, 
while the meshlist.txt. and objects.txt holds the meshes and objects, 
both uses spaces to separate the properties.



meshlist.txt is composed of 1 mesh per line, and the line structure is:
meshname  name_of_the_mesh_file1.ply

example:
virus1    virus_super_low_res_XYZ.ply
asteroid1 asteroid_xyz.ply




objects.txt is composed of lines for each meshtipe, 
each line can create 1 or more objects in the game, the line structure is:

meshname number_of_objects posX posY posZ scale isRandom? rangeX rangeY rangeZ rangeScale

example:
asteroid1 1 2.0 -3.0 -2.0 1.0 false 0.0 0.0 0.0 0.0
asteroid2 10 0.0 0.0 0.0 0.0 true 10.0 10.0 10.0 2.0

Line 1 will insert 1 object of the mesh type asteroid1 at the positonXYZ 2.0, -3.0, -2.0 with scale 1.0.
Line 2 will insert 10 objects of the type asteroid 2 at a random position of XYZ from -10.0 to 10.0 
for each cordinate with a random scale of 0.0 to 2.0