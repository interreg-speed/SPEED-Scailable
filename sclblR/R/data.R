#' Prices of 5606 houses in the Netherlands
#'
#' Dataset containing 5606 houses listed for sale in the ten biggest cities in the Netherlands
#' on November 15, 2019.
#'
#' The dummy coded cities are:
#'
#' Almere (ALM)
#' Amsterdam (AMS)
#' Breda (BRE)
#' The Hague (DHA)
#' Eindhoven (EIN)
#' Groningen (GRO)
#' Nijmegen (NIJ)
#' Rotterdam (ROT)
#' Tilburg (TIL)
#' Utrecht (UTR)
#'
#'
#' @format A data frame with 5606 rows and 16 variables:
#' \describe{
#'     \item{price}{The asking price}
#'     \item{volume}{The volume in m3}
#'     \item{yard}{The size of the yard in m2}
#'     \item{area}{The living area in m2}
#'     \item{rooms}{The number of rooms}
#'     \item{sleeping}{The number of bedrooms}
#'     \item{iso}{Isolated? 0 = no, 1 = yes}
#'     \item{hood}{Neighborhood name (not used in demo analysis)}
#'     \item{garden}{Garden present? 0 = no, 1 = yes}
#'     \item{balcony}{Balcony present? 0 = no, 1 = yes}
#'     \item{garage}{Garage present? 0 = no, 1 = yes}
#'     \item{age}{Age of the building in years since 2019}
#'     \item{openh}{Fireplace present? 0 = no, 1 = yes}
#'     \item{paint}{Recently painted? 0 = no, 1 = yes}
#'     \item{days}{Posted in days since 15-11-2019}
#'     \item{mALM}{Almere (ALM)}
#'     \item{mAMS}{Amsterdam (AMS)}
#'     \item{mBRE}{Breda (BRE)}
#'     \item{mDHA}{The Hague (DHA)}
#'     \item{mEIN}{Eindhoven (EIN)}
#'     \item{mGRO}{Groningen (GRO)}
#'     \item{mNIJ}{Nijmegen (NIJ)}
#'     \item{mROT}{Rotterdam (ROT)}
#'     \item{mTIL}{Tilburg (TIL)}
#'     \item{mUTR}{Utrecht (UTR)}
#' }
"houses"
