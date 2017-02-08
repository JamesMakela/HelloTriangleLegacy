# HelloTriangleLegacy
A simple-as-possible example of an OpenGL Program using legacy
OpenGL 2.1 (GLSL 1.2)

Okay, I decided it was time to finally learn some of the ins
and outs of OpenGL software development, and of course
there were quite a few instructional online resources
willing to give a few hints here and there. There happened
to be a few online tutorials showcasing an OpenGL version
of a "hello world" program named hello_triangle.cpp, which
would put a triangle in a window, rendered by OpenGL.

These tutorials are great, or they would have been if I was
using a modern PC with a modern graphics card.  You see,
these tutorials were focused on OpenGL 3.3 or higher
(as of about Jan, 2017).

As it was, I had an old laptop (circa ~2009) with an old
video card (GeForce 7150M / nForce 630M).  And I was running
Linux, so the newest drivers I could get only supported
OpenGL 2.1 (GLSL version 1.20)

So I went through a lot of pain and effort, and took a lot
of time going through the legacy specifications that I could
find online.  And I was finally able to develop a version
of the hello_triangle demo program that worked with the older
APIs.

It's funny that I had to cobble this together in bits and
pieces, that there were no legacy instructional tutorials
that would have helped me through all this.  Surely I can't
be the only person out there with old hardware who wants to
get into OpenGL.

So I hope that the next developer who finds him/herself
in a similar situation will find this and have at least a
slightly easier time than I did.
