<b>Name:</b> Yanisa Srisa-ard

<b>UCID:</b> 30150196

The source files have been uploaded as a zip file into the dropbox.

<u>The program includes:</u>
* Bezier and B-Spline Curves
* Viewing Pipeline
* Surface of Revolution
* Tensor Product Surfaces
* Extension??? maybe

<u>General keyboard controls:</u>
* <b>RIGHT ARROW KEY:</b> Go to the next assignment part/exercise.
* <b>LEFT ARROW KEY:</b> Go to the previous assignment part/exercise.
* <b>UP ARROW KEY:</b> Toggle between Bezier and B-Spline curves (only for part 1 and 2). 

<u>Bezier & B-Spline mouse and keyboard controls:</u>
* <b>R KEY:</b> Reset control points.
* <b>BACKSPACE KEY:</b> Delete a selected control point.
* <b>LEFT MOUSE CLICK:</b> Add a control point at the cursor's location OR select an exisiting control point.
    * If a control point has been selected, it can be moved according to the cursor's position (you MUST left click again to place the control point at your desired location), or deleted (press backspace key).

<u>Viewing pipeline controls:</u>
* <b>R KEY:</b> Reset camera.
* <b>MOUSE SCROLL:</b> Change camera distance.
* <b>LEFT MOUSE CLICK & MOVE:</b> Rotate camera angle (you MUST left click again to keep the camera at a certain angle).

<u>Surface of revolution controls:</u>
* <b>DOWN ARROW KEY:</b> Toggle between the wireframe and solid surface view.
* <b>MOUSE SCROLL:</b> Change camera distance.
* <b>LEFT MOUSE CLICK & MOVE:</b> Rotate camera angle (you MUST left click again to keep the camera at a certain angle).


<u>Notes:</u>
* The program outputs into the console when a key is pressed (e.g. changed from bezier to b-spline curve, added control point, reset control points, etc.).
* The Bezier curve is red and the B-Spline curve is blue.
--------------------------

<b>Operating System:</b> macOS

<b>CMake version:</b> 3.30.3

<b>Note:</b> I wrote my original code using VScode on my mac laptop. However, I have edited it using the computers in the lab room, therefore it runs perfectly on the computers' <b>VScode</b>, not the regular Visual Studio (not tested so not confirmed).

<u>Instructions to compile the program:</u>
1. Open the folder (453-skeleton-a3-completed) using VScode.
2. Select the 'Unspecified' kit (if needed).
3. Build the program using CMake (exit code should be 0).
4. Open a new terminal.
5. Enter the following: 
```bash
cd build
./453-skeleton
```