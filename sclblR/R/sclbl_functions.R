#' Generate Scailable BART model
#'
#' Takes a fitted BART model and generates a zip file
#' from it that can then be uploaded to scailable.net
#'
#' @param bart_model fitted bart model
#' @param filename name of the generated zip file (default: "bart.zip")
#' @param upload whether to automatically upload the model to scailable.net or not (default: FALSE)
#' @param sclbl_config_file location of config file containing the scailable API endpoint URL and
#'                          scailable credentials (default: sclbl_config.yml)
#'
#' @examples
#'
#' \dontrun{
#'   load("bart_model.RData")
#'   save_bart(bart_model, "bart.zip", upload = FALSE)
#' }
#'
#' @import jsonlite
#' @import stringr
#' @import zip
#' @import httr
#' @import uuid
#' @import yaml
#' @export
save_bart <- function(bart_model, filename = "bart.zip", upload = FALSE,
                      sclbl_config_file = "sclbl_config.yml") {

  # load configuration file
  config = yaml::yaml.load_file(sclbl_config_file)

  # create temporary directory
  dir.create("tmp", showWarnings = FALSE)

  # Create cutpoints header file
  cutpoints_string <-
    jsonlite::toJSON(bart_model$treedraws$cutpoints)
  cutpoints_string <-
    stringr::str_replace_all(cutpoints_string, "[A-Za-z\":]", '')
  cutpoints_string <-
    stringr::str_replace_all(cutpoints_string, "\\[", '{')
  cutpoints_string <-
    stringr::str_replace_all(cutpoints_string, "\\]", '}')
  file_con <- file("tmp/cutpoints_header.h")
  writeLines(
    c(
      "static std::vector<std::vector<double> > xi = ",
      cutpoints_string,
      ";"
    ),
    file_con,
    sep = ""
  )
  close(file_con)

  # Create tree header file
  file_con <- file("tmp/trees_header.h")
  writeLines(
    c(
      "static std::string itv = R\"~~~~(",
      bart_model$treedraws$trees,
      ")~~~~\";"
    ),
    file_con
  )
  close(file_con)

  # Create mu header file
  file_con <- file("tmp/mu_header.h")
  writeLines(c("static double mu = ", bart_model$mu, ";"), file_con, sep = "")
  close(file_con)

  # zip all three files
  zip::zipr(filename,
            c(
              "tmp/cutpoints_header.h",
              "tmp/trees_header.h",
              "tmp/mu_header.h"
            ))

  # delete temporary directory
  unlink("tmp", recursive = TRUE)

  # optionally upload model file directly to scailable.net
  if(isTRUE(upload)) {

    upload_endpoint <- config$API$endpoint

    file_path <- normalizePath(filename)

    uuid_file_name <- uuid::UUIDgenerate()

    response = httr::POST(upload_endpoint,
                          body=list(name=paste0(uuid_file_name,".zip"),
                          media=httr::upload_file(filename)),
                          encode="multipart"
    )
    uuid_server_response <- httr::content(response, "parsed")[["uuid"]]
    message(paste("Task UUID returned by server:", uuid_server_response))
    return(uuid_server_response)
  }
  return(normalizePath(filename))
}
