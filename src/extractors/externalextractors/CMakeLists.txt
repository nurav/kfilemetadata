install(
    DIRECTORY pdfextractor
    DESTINATION ${KF5_LIBEXEC_INSTALL_DIR}/kfilemetadata/externalextractors
    PATTERN "*.py"
    PERMISSIONS OWNER_READ OWNER_WRITE WORLD_EXECUTE
    )
