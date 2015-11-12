SET(DICOM_VISUALISATION_HEADER_FILES glErrorCheck_inline.h global.h histogram.DrawFunc.h histogramKeyboardFunc.h histogramMouseFunc.h importDicomFile.h menu.h openGLStatus.h rescalVolume.h segementationMethods.h viewSegmentedData.h)

add_library(HEADER_FILES ${DICOM_VISUALISATION_HEADER_FILES})
