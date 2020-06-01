Sys.setenv("R_TESTS" = "")

library(testthat)
library(sclblR)

test_check("sclblR")
