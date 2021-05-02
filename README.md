# p-adic-visualization
Utility for generating visualizations of analytic functions on the p-adic integers Z_p.

Use ./image in terminal to get started.

Currently plots Z_p as a tree with specified number of "generations". Each branch of the tree represents an
additive coset, j+p^kZ_p. Colors are used to indicate the value of j.

The utility lets you define a power series by its coefficients, then plots values up to desired coset accuracy.

Also includes templated class implementation of the p-adic numbers. Operator overloads for +, -, * and more. Can 
compute p-adic inverses up to desired number of digits, as well as p-adic order and norms.
