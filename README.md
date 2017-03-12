# ShapeManipulation
Manipulate object through control points
对图像进行阈值分割，将分割后的轮廓进行网格化，载入生成的网格模型，并贴上原图作为纹理。
用户可以任意添加控制点，对控制点进行拖动以对网格进行变形。
变形算法使用移动最小二乘法（MLS, Moving Least Square），参见Weng Y, Xu W, Wu Y, et al. 2D shape deformation using nonlinear least squares optimization[J]. The Visual Computer, 2006, 22(9):653-660.