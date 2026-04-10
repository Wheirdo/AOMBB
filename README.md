# AOMBB
Arbitrarily Oriented Minimum Bounding Box

A Bounding Box is the smallest possible box that fully encompasses an N-Dimensional object. Specifically, the minimum volume for any arbitrary 3D shape, otherwise known as minimum-volume arbitrarily-oriented bounding box of a three-dimensional object. 
This project was an exploration of the paper “Finding minimal enclosing boxes” by Joseph O'Rourke[^1], where he describes an algorithm to find such a bounding box in quadratic time.
This was my attempt at adapting his paper into C++, using OpenGL and GLSL for shading.

Before starting the actual algorithm, the paper makes the assumption that all inputed 3D Objects have been transformed its convex hull, which is a shape that contains all vertices within. 
I used an online converter[^2][^3] to transform the inputed OBJ files into convex hulls of themselves.

The Bounding Box algorithm itself starts with constructing a Gaussian Sphere, which is a unit sphere where each region is mapped to a vertex on the input polyhedra, where the distance from the origin is mapped to the unit normal vector that starts at the origin and goes through the vector in the input polyhedra.
The paper uses loops through the Gaussian Sphere’s points, 6 of which (that are antipodal) will be used to make the final bounding box. 
Here is where I hit my biggest major roadblock: there is almost no information about Gaussian Sphere’s on the internet. The papers he references in his paper have no mention of it, and searching the internet mainly returns results for Gaussian Surface, a law of electromagnetism, or Spherical Gaussians, which is something used for coloring spheres. You can see my attempts to create a Gaussian Sphere as its the third shape displayed (along with the code, found in the “spherify” function in Shape.cpp).

Fortunately, the use of a Gaussian Surface is an optimization to the overall brute force algorithm described, which I could still implement. The idea behind the algorithm is a concept called ‘rotating calipers’, which is a way to solve optimization problems like this one. The bulk of the algorithm is looping around the X axis (from 0°to 90°) and the Z axis (from 0°to 180°). For each of those degrees, loop through all the vertices and rotate them according to the X and Z rotations. This loop is checking for the minimum and maximum x, y and z coordinates. We solve for the volume of this bounding box against the running smallest bounding box, and update accordingly. After all of that, we have the bounding box with the smallest volume, but there are a few more things to do. For one, since this is an arbitrary oriented bounding box, we need to rotate the whole polyhedra to the same orientation as the bounding box we found. Similarly, we need to move both the bounding box and the polyhedra to the center.

I ran into a few problems whilst trying to adapt this paper. The Gaussian Sphere proved to be the largest, since finding any additional information on the paper proved difficult. While his paper is quite famous (being in the top 10% of cited articles from the time[^4], along with being on the Wikipedia page on Bounding Boxes[^5]), the search for information was a struggle regardless. Furthermore, this paper is presented as a geometry problem, not a computing one, which meant converting his math into C++ proved a daunting task. Finally, since my final implementation doesn’t have the optimizations, it works out to be closer to cubic time, not quadratic. Regardless, the bounding boxes found were significantly smaller than the axis aligned bounding box, for objects where improvements could be made. 

Overall, I'm happy with how it turned out. It acted as both an opportunity to learn more about Bounding Boxes, while also letting me learn more about GLSL and graphics.

## Results

| Object | Axis Aligned Bounding Box | Arbitrarily Oriented Bounding Box
| --- | --- | --- |
| Bunny | 0.76857 | 0.639487 |
| Teapot | 0.304789 | 0.248808 |
| Ashtray | 0.203279 | 0.203279 |
| Tetrahedron | 0.885973 | 0.400584 |
| Molecule | 0.207685 | 0.207685 |
| Sphere | 1.0 | 0.87865 |
| Great Dodecahedron | 1.0 | 1.0 |


## How to run

- Install Visual Studio and Visual C++
- Install CMake
- Download GLM, GLFW, and FLEW

- Add the following envoirments variables:
  - **GLM_INCLUDE_DIR** to **/path/to/GLM**
  - **GLFW_DIR** to **/path/to/GLFW**
  - **GLEW_DIR** to **/path/to/GLEW**

- Run CMake
  - Click **Browse Source...** and find **CMakeLists.txt**
  - In **Where to build the binaries**, copy-paste the text in **Where is the source code** and add **/build**
    - (so **/Path/Here** would become **/Path/Here/build**)
  - Click **Configure**, select your version of visual studio, then click **Finish**
  - Click **Generate**, and then close Cmake.

- Open Visual Studio, and run the .sln file.
- Right click on the **MinVol** inside Visual Studio, select **Properties**
- Go to **Debugging**, then **Command Arguments**, then type **../resources**, then click OK

Then hit the green run button and you should be good!


[^1]: https://www.science.smith.edu/~jorourke/Papers/MinVolBox.pdf
[^2]: https://www.weiy.city/functions/3D-Model-Editor/
[^3]: https://www.weiy.city/3d-files-converter/
[^4]: https://link.springer.com/article/10.1007/BF00991005/metrics
[^5]: https://en.wikipedia.org/wiki/Minimum_bounding_box
