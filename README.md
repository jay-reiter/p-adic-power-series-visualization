# p-adic-visualization

Utility for generating visualizations of analytic functions on the p-adic integers Z_p.

Use ./image in terminal to get started.

Currently plots Z_p as a tree with specified number of "generations". Each branch of the tree represents an
additive coset, j+p^kZ_p. Colors are used to indicate the value of j.

The utility lets you define a power series by its coefficients, then plots values up to desired coset accuracy.

Also includes templated class implementation of the p-adic numbers. Operator overloads for +, -, * and more. Can 
compute p-adic inverses up to desired number of digits, as well as p-adic order and norms.

# Plots in this repo:

Check out the plots of p-adic EXP and SIN and COS in the .gif files.

The green line represents x and the pink EXP(x) in exp__low_res.gif

The green line represents x and the pink COS(x) and the blue SIN(x) in sin_cos__low_res.gif

# Example usage:

Create a function "p_adic<p> a(unsigned)" to define coefficients of a power series.
Use this function with "power_series(...)" in "thread_process_image(...)" in p-adic_draw.cpp.

From the command line, run: ./image p_adic <width> <p> <children> [target_file_name]

(I reccomend ./image p_adic 5000 7 5)
  
This will take a while to generate each frame of the gif (around an hour running 7 threads on my machine, with i5-9400F at 2.90 GHz, 6 cores). When its done, the images are stored in ./pngs/ . My 7^5=2401 images at 5000x5000 each take up as much as a gigabyte. Compile and run ./rename_files.c to prepare file names (this erases their descriptive names).

Convert to gif: ffmpeg -i pngs/%04d.png out.gif

Resize gif: ffmpeg -y -i out.gif -pix_fmt rgb8 -r 10 -s 380x380 out__low_res.gif

