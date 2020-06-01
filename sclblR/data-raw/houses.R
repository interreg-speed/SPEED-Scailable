houses <- read.csv(file = "data-raw/houses.csv")

usethis::use_data(houses, overwrite = TRUE)
