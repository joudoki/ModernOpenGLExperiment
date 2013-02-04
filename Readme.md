#Modern OpenGL Experiment
Ryan Lewellen
December 2012 - February 2013

##Purpose
I've had a fair bit of experience with the traditional, fixed-pipeline variant of OpenGL; however, after taking CSCI441
last fall, I decided to take the time (and the plunge) to learn a bit about the modern variant. This has required quite
a bit of reading through the OpenGL specs but ultimately I've learned quite a bit.

##Objectives
I wanted to get experience working with OpenGL 3.3 in the Core Profile mode. To that end, I've ended up investing the
following topics:

* Context Acquisition with GLFW's context hints
* Function Acquisition via a custom loader (like GLEW), except that it only loads valid 3.3 core commands.
  I generated this using glLoadGen 1.0.3, and I quite like the results it has given - mainly that using the custom
  loader didn't pollute the default namespace with useless deprecated commands, while still working with GLFW
* Managing data in Vertex Buffers and Vertex Array Buffers
* Simple GLSL for Vertex and Fragment Shaders, as well as associated management via Programs

##Links
* http://www.glfw.org/
* https://bitbucket.org/alfonse/glloadgen
* http://www.opengl.org/registry/doc/glspec33.core.20100311.pdf