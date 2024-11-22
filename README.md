<b>Name:</b> Yanisa Srisa-ard

<b>UCID:</b> 30150196

The source files have been uploaded as a zip file into the dropbox.

<u>The program includes:</u>
* Bezier and B-Spline Curves
* Viewing Pipeline
* Surface of Revolution
* Tensor Product Surfaces
* Bonus Part 1: Camera Extension

<u>General keyboard controls:</u>
* <b>RIGHT ARROW KEY:</b> Go to the next assignment part/exercise.
* <b>LEFT ARROW KEY:</b> Go to the previous assignment part/exercise.

<u>PART 1: Bezier & B-Spline mouse and keyboard controls:</u>
* <b>UP ARROW KEY:</b> Toggle between Bezier and B-Spline curves.
* <b>R KEY:</b> Reset control points.
* <b>BACKSPACE KEY:</b> Delete a selected control point.
* <b>LEFT MOUSE CLICK:</b> Add a control point at the cursor's location OR select an exisiting control point.
    * If a control point has been selected, it can be moved according to the cursor's position (you MUST left click again to place the control point at your desired location), or deleted (press backspace key).

<u>PART 2: Viewing Pipeline controls:</u>
* <b>UP ARROW KEY:</b> Toggle between Bezier and B-Spline curves.

<u>PART 3: Surface of Revolution controls:</u>
* <b>W KEY:</b> Toggle between the wireframe and solid surface view.

<u>PART 4: Tensor Product Surfaces controls:</u>
* <b>W KEY:</b> Toggle between the wireframe and solid surface view.

<u>Camera mouse and keyboard controls (for parts 2,3,4):</u>
* <b>R KEY:</b> Reset camera.
* <b>MOUSE SCROLL:</b> Change camera distance.
* <b>LEFT MOUSE CLICK & MOVE:</b> Rotate camera angle (you MUST left click again to keep the camera at a certain angle).

<u>Bonus Part 1 controls (for parts 2,3,4):</u>
* <b>O KEY:</b> Toggle between Orthographic and Perspective camera. 
* <b>F KEY:</b> [For Perspective camera only] Increase the FOV. <b>HOLD SHIFT KEY</b> to decrease.
* <b>N KEY:</b> Increase the near clip distance. <b>HOLD SHIFT KEY</b> to decrease.
* <b>A KEY:</b> Increase the far clip distance. <b>HOLD SHIFT KEY</b> to decrease.
* <b>S KEY:</b> [For Orthographic camera only] Increase the ortho scale. <b>HOLD SHIFT KEY</b> to decrease.

<u>Notes:</u>
* The program outputs into the console when a key is pressed (e.g. changed from bezier to b-spline curve, camera reset, etc.).
* The Bezier curve is red and the B-Spline curve is blue.

--------------------------

<b>Operating System:</b> macOS

<b>CMake version:</b> 3.30.3

<b>Note:</b> I wrote my original code using VScode on my mac laptop. However, I have edited it using the computers in the lab room, therefore it runs perfectly on the computers' <b>VScode</b>, not the regular Visual Studio (not tested so not confirmed).

<u>Instructions to compile the program:</u>
1. Open the folder (cpsc=453-curves-surfaces) using VScode.
2. Select the 'Unspecified' kit (if needed).
3. Build the program using CMake (exit code should be 0).
4. Open a new terminal.
5. Enter the following: 
```bash
cd build
./453-skeleton
```

If you are having troubles with building (running cmake with the output dropdown / build button at the bottom toolbar), enter the following into the terminal:
```bash
cd build
cmake ..
make
./453-skeleton
```