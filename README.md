# Periodic Noise Removing Filter

This repository contains a modified version of the code from the OpenCV documentation tutorial on periodic noise removal filters. The main purpose of this modification is to experiment with the effect of replacing the sharp cut-off circle filter with a Gaussian function filter for noise removal. Additionally, the process of automating the identification and removal of frequencies responsible for periodic noise has been implemented.

The main function loads an input image, applies various processing steps including obtaining the Power Spectral Density (PSD) of the image, thresholding, morphological closing, and finding bounding boxes for noise regions. It then generates two types of filters: one based on the automated bounding boxes (H), and another using Gaussian functions (G). These filters are applied to the input image to remove noise, and the results are displayed.

Furthermore, the repository includes utility functions for image processing and loading, as well as functions for generating and applying the filters.

Lastly, the repository provides functionality to visualize and save the results, including the PSD of the input image, the generated filters (FilterH and FilterG), and the deblurred images (DeblurringH and DeblurringG).
