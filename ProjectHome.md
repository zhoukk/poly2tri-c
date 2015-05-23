This is a C port of the [poly2tri](http://code.google.com/p/poly2tri/) library - a fast and powerful library for computing 2D constrained Delaunay triangulations. Instead of the standard C++ library (which included some utilities and template-based data structures), this port depends on [GLib](http://developer.gnome.org/glib/) for it's data structures and some of it's utilities.

This library will also have an additional feature the source didn't have - it will have an implementation of the [Delaunay Terminator algorithm](http://www.cs.berkeley.edu/~jrs/papers/2dj.pdf) for mesh refinement.

This port was done as a part of Google Summer of Code 2011, for the GIMP project. This port will be sent to be integrated with the original poly2tri project, once the work on it is done.

Any comments/suggestions will be appreciated!

**June 5th 2011:** The port is now finished, although the C parts of the API are not yet completely documented, and some places are still using "plain C" instead of the glib types. Implementing mesh refinement starts now!

**July 28th 2011:** The refinement algorithm (not part of the original poly2tri) is usable now, and there is even some nice memory management. However, it does not seem to converge or be of good quality. Again, it should be enough for many projects if they fine tune the parameters of the Delaunay Terminator, and set the p2t\_refine\_steps environment variable. Still, I would recommend anyone who wants to use it, to wait for proper documentation and fixing.

**April 26th 2012:** The library was re-implemented with a more correct version of the algorithm and some new data structures. Also, the code was moved into a new git repository instead of the old SVN repository. Therefore, **The library code is now being updated all the time untill the new code is finally comitted**. This will take several days

**July 7th 2012:** The library is useable! Many improvements were done to solve bugs in the algorithm and it now works for most simple use cases!