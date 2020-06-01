sclblR: scailable models in R
==============================

Overview
--------

R package facilitating the export of fitted statistical models to Scailable.net

Installation
------------

To install the development version (requires the devtools package):

```R
install.packages("devtools")
devtools::install_github('scailable/sclblR')
```

When working on or extending the package, clone its [GitHub repository](https://github.com/scailable/sclblR), then do:

```R
install.packages("devtools")
devtools::install_deps(dependencies = TRUE)
devtools::build()
devtools::reload()
```

clean and rebuild...

Example
-------

```R
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
if (file.exists("demo/bart.zip"))  file.remove("demo/bart.zip")
```

Maintainers
-----------

Robin van Emden / Maurits Kaptein: author, maintainer*

\* [Scailable.](https://www.scailable.net/) 
