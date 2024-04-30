# Periodic Noise Removing Filter

This repository contains a modified version of the code from the OpenCV documentation tutorial on periodic noise removal filters. The main purpose of this modification is to experiment with the effect of replacing the sharp cut-off circle filter with a Gaussian function filter for noise removal. Additionally, the process of automating the identification and removal of frequencies responsible for periodic noise has been implemented.

The main function loads an input image, applies various processing steps including obtaining the Power Spectral Density (PSD) of the image, thresholding, morphological closing, and finding bounding boxes for noise regions. It then generates two types of filters: one based on the automated bounding boxes (H), and another using Gaussian functions (G). These filters are applied to the input image to remove noise, and the results are displayed.

Furthermore, experimental results suggest that the Gaussian function filter (G) tends to provide slightly improved noise removal compared to the original sharp cut-off circle filter (H).

![image](https://github.com/kamil-janusz-kalinowski/Periodic-Noise-Removing-Filter/assets/143912944/1ec574f7-cde1-44d3-a636-548dc2aa6b16)
