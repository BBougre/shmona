

# Sort 8 bit number by ascending count of 1's

mat <- 
	matrix(data = as.numeric(intToBits(0:255)), ncol = 256)[1:8, ]
mat

n_1 <- colSums(mat)
names(n_1) <- 0:(ncol(mat)-1)
n_1 <- sort(n_1)
table(n_1)
rythm <- as.integer(names(n_1))

rythm

cat('{', 
	paste(rythm, ','),
	'}', 
	'\n')