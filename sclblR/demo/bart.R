library(sclblR)
library(BART)

# Define y and X for BART model
X <- houses[,c(-1)]
y <- houses$price

# Ignore "neighborhood" variable
X$hood <- NULL

# Fit thinned BART model
bart_model <- BART::wbart(X,y,ndpost=1000, nkeeptreedraws=200)

# Save BART model for later upload to Scailable
save_bart(bart_model, "demo/bart.zip", upload = TRUE)

# Clean up after demo (deletes saved file, comment out when you want to keep it)
# if (file.exists("demo/bart.zip"))  file.remove("demo/bart.zip")
