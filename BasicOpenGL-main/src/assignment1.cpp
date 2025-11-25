#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * Converts an RGBA image buffer to grayscale
 * 
 * @param input - Pointer to the original RGBA image buffer
 * @param width - Width of the image in pixels
 * @param height - Height of the image in pixels
 * @param channels - Number of channels in the input image (should be 3 or 4)
 * @return Pointer to newly allocated 1-channel grayscale buffer, or NULL on failure
 */
unsigned char* Grayscale_convert(unsigned char* input, int width, int height, int channels)
{
    if (input == nullptr || width <= 0 || height <= 0 || channels < 3)
    {
        return nullptr;
    }

    // Allocate memory for grayscale output (1 channel per pixel)
    unsigned char* grayscale = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    
    if (grayscale == nullptr)
    {
        return nullptr;
    }

    // Convert each pixel to grayscale using the proper RGB formula
    for (int i = 0; i < width * height; i++)
    {
        int pixel_index = i * channels;
        unsigned char r = input[pixel_index];
        unsigned char g = input[pixel_index + 1];
        unsigned char b = input[pixel_index + 2];
        
        // Apply the grayscale conversion formula: gray = 0.299*r + 0.587*g + 0.114*b
        float gray_float = 0.299f * r + 0.587f * g + 0.114f * b;
        
        // Clip to 0-255 range (though the formula should already produce values in this range)
        int gray_int = (int)(gray_float + 0.5f); // Round to nearest integer
        grayscale[i] = (unsigned char)std::max(0, std::min(255, gray_int));
    }

    return grayscale;
}

/**
 * Applies Gaussian blur to a grayscale image using a 3x3 kernel
 * 
 * @param gray - Pointer to the grayscale image buffer
 * @param width - Width of the image in pixels
 * @param height - Height of the image in pixels
 * @return Pointer to newly allocated blurred grayscale buffer, or NULL on failure
 */
unsigned char* GaussianBlur3x3(unsigned char* gray, int width, int height)
{
    if (gray == nullptr || width <= 0 || height <= 0)
    {
        return nullptr;
    }

    // 3x3 Gaussian kernel (sigma = 0.8)
    float kernel[3][3] = {
        {1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f},
        {2.0f/16.0f, 4.0f/16.0f, 2.0f/16.0f},
        {1.0f/16.0f, 2.0f/16.0f, 1.0f/16.0f}
    };

    unsigned char* blurred = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    if (blurred == nullptr)
    {
        return nullptr;
    }

    int kernel_size = 3;
    int offset = kernel_size / 2;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float sum = 0.0f;

            for (int ky = 0; ky < kernel_size; ky++)
            {
                for (int kx = 0; kx < kernel_size; kx++)
                {
                    int px = x + kx - offset;
                    int py = y + ky - offset;

                    // Handle boundary by clamping
                    px = std::max(0, std::min(width - 1, px));
                    py = std::max(0, std::min(height - 1, py));

                    sum += gray[py * width + px] * kernel[ky][kx];
                }
            }

            // Clamp to 0-255
            int value = (int)(sum + 0.5f);
            blurred[y * width + x] = (unsigned char)std::max(0, std::min(255, value));
        }
    }

    return blurred;
}

/**
 * Applies Gaussian blur to a grayscale image using a 5x5 kernel
 * 
 * @param gray - Pointer to the grayscale image buffer
 * @param width - Width of the image in pixels
 * @param height - Height of the image in pixels
 * @return Pointer to newly allocated blurred grayscale buffer, or NULL on failure
 */
unsigned char* GaussianBlur5x5(unsigned char* gray, int width, int height)
{
    if (gray == nullptr || width <= 0 || height <= 0)
    {
        return nullptr;
    }

    // 5x5 Gaussian kernel (sigma = 1.0)
    float kernel[5][5] = {
        {1.0f/273.0f, 4.0f/273.0f, 7.0f/273.0f, 4.0f/273.0f, 1.0f/273.0f},
        {4.0f/273.0f, 16.0f/273.0f, 26.0f/273.0f, 16.0f/273.0f, 4.0f/273.0f},
        {7.0f/273.0f, 26.0f/273.0f, 41.0f/273.0f, 26.0f/273.0f, 7.0f/273.0f},
        {4.0f/273.0f, 16.0f/273.0f, 26.0f/273.0f, 16.0f/273.0f, 4.0f/273.0f},
        {1.0f/273.0f, 4.0f/273.0f, 7.0f/273.0f, 4.0f/273.0f, 1.0f/273.0f}
    };

    unsigned char* blurred = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    if (blurred == nullptr)
    {
        return nullptr;
    }

    int kernel_size = 5;
    int offset = kernel_size / 2;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float sum = 0.0f;

            for (int ky = 0; ky < kernel_size; ky++)
            {
                for (int kx = 0; kx < kernel_size; kx++)
                {
                    int px = x + kx - offset;
                    int py = y + ky - offset;

                    // Handle boundary by clamping
                    px = std::max(0, std::min(width - 1, px));
                    py = std::max(0, std::min(height - 1, py));

                    sum += gray[py * width + px] * kernel[ky][kx];
                }
            }

            // Clamp to 0-255
            int value = (int)(sum + 0.5f);
            blurred[y * width + x] = (unsigned char)std::max(0, std::min(255, value));
        }
    }

    return blurred;
}

/**
 * Applies Gaussian blur to a grayscale image using a 7x7 kernel
 * 
 * @param gray - Pointer to the grayscale image buffer
 * @param width - Width of the image in pixels
 * @param height - Height of the image in pixels
 * @return Pointer to newly allocated blurred grayscale buffer, or NULL on failure
 */
unsigned char* GaussianBlur7x7(unsigned char* gray, int width, int height)
{
    if (gray == nullptr || width <= 0 || height <= 0)
    {
        return nullptr;
    }

    // 7x7 Gaussian kernel (sigma = 1.4)
    float kernel[7][7] = {
        {0.0009f, 0.0031f, 0.0067f, 0.0089f, 0.0067f, 0.0031f, 0.0009f},
        {0.0031f, 0.0113f, 0.0242f, 0.0322f, 0.0242f, 0.0113f, 0.0031f},
        {0.0067f, 0.0242f, 0.0520f, 0.0692f, 0.0520f, 0.0242f, 0.0067f},
        {0.0089f, 0.0322f, 0.0692f, 0.0920f, 0.0692f, 0.0322f, 0.0089f},
        {0.0067f, 0.0242f, 0.0520f, 0.0692f, 0.0520f, 0.0242f, 0.0067f},
        {0.0031f, 0.0113f, 0.0242f, 0.0322f, 0.0242f, 0.0113f, 0.0031f},
        {0.0009f, 0.0031f, 0.0067f, 0.0089f, 0.0067f, 0.0031f, 0.0009f}
    };
    
    // Normalize kernel to ensure sum = 1.0
    float kernel_sum = 0.0f;
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            kernel_sum += kernel[i][j];
        }
    }
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            kernel[i][j] /= kernel_sum;
        }
    }

    unsigned char* blurred = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    if (blurred == nullptr)
    {
        return nullptr;
    }

    int kernel_size = 7;
    int offset = kernel_size / 2;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float sum = 0.0f;

            for (int ky = 0; ky < kernel_size; ky++)
            {
                for (int kx = 0; kx < kernel_size; kx++)
                {
                    int px = x + kx - offset;
                    int py = y + ky - offset;

                    // Handle boundary by clamping
                    px = std::max(0, std::min(width - 1, px));
                    py = std::max(0, std::min(height - 1, py));

                    sum += gray[py * width + px] * kernel[ky][kx];
                }
            }

            // Clamp to 0-255
            int value = (int)(sum + 0.5f);
            blurred[y * width + x] = (unsigned char)std::max(0, std::min(255, value));
        }
    }

    return blurred;
}

// Wrapper function - uses 5x5 by default
unsigned char* GaussianBlur(unsigned char* gray, int width, int height)
{
    return GaussianBlur5x5(gray, width, height);
}

/**
 * Computes gradient magnitude and direction using Central Difference kernels
 * Based on: ∂I/∂x ≈ (I[x+ε, y] - I[x-ε, y]) / ε and ∂I/∂y ≈ (I[x, y+ε] - I[x, y-ε]) / ε
 * Kernels: [-1 0 1] for x-direction and [-1; 0; 1] for y-direction
 * 
 * @param blurred - Pointer to the blurred grayscale image
 * @param width - Width of the image in pixels
 * @param height - Height of the image in pixels
 * @param gradientMagnitude - Output array for gradient magnitudes (must be pre-allocated)
 * @param gradientDirection - Output array for gradient directions in radians (must be pre-allocated)
 */
void Gradient(
    unsigned char* blurred,
    int width,
    int height,
    float* gradientMagnitude,
    float* gradientDirection
)
{
    if (blurred == nullptr || gradientMagnitude == nullptr || gradientDirection == nullptr)
    {
        return;
    }

    // Central difference kernels (ε = 1)
    // x-direction: [-1 0 1] (horizontal)
    // y-direction: [-1; 0; 1] (vertical)
    
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float gx = 0.0f;
            float gy = 0.0f;

            // Calculate Gx using central difference: [-1 0 1]
            // ∂I/∂x ≈ (I[x+ε, y] - I[x-ε, y]) / ε
            int px_left = std::max(0, x - 1);
            int px_right = std::min(width - 1, x + 1);
            gx = (float)blurred[y * width + px_right] - (float)blurred[y * width + px_left];

            // Calculate Gy using central difference: [-1; 0; 1]
            // ∂I/∂y ≈ (I[x, y+ε] - I[x, y-ε]) / ε
            int py_top = std::max(0, y - 1);
            int py_bottom = std::min(height - 1, y + 1);
            gy = (float)blurred[py_bottom * width + x] - (float)blurred[py_top * width + x];

            // Compute magnitude
            float magnitude = std::sqrt(gx * gx + gy * gy);
            gradientMagnitude[y * width + x] = magnitude;

            // Compute direction in radians
            float angle = std::atan2(gy, gx);
            gradientDirection[y * width + x] = angle;
        }
    }
}

/**
 * Performs non-maximum suppression to thin edges
 * 
 * @param gradientMagnitude - Array of gradient magnitudes
 * @param gradientDirection - Array of gradient directions in radians
 * @param width - Width of the image in pixels
 * @param height - Height of the image in pixels
 * @return Pointer to newly allocated thinned edge buffer, or NULL on failure
 */
unsigned char* NonMaxSuppression(
    float* gradientMagnitude,
    float* gradientDirection,
    int width,
    int height
)
{
    if (gradientMagnitude == nullptr || gradientDirection == nullptr)
    {
        return nullptr;
    }

    // Find maximum gradient magnitude for normalization
    float maxMag = 0.0f;
    for (int i = 0; i < width * height; i++)
    {
        if (gradientMagnitude[i] > maxMag)
        {
            maxMag = gradientMagnitude[i];
        }
    }

    unsigned char* thinned = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    if (thinned == nullptr)
    {
        return nullptr;
    }

    // Initialize to zero
    memset(thinned, 0, width * height * sizeof(unsigned char));

    // Normalize factor (avoid division by zero)
    float normalizeFactor = (maxMag > 0.0f) ? (255.0f / maxMag) : 1.0f;

    for (int y = 1; y < height - 1; y++)
    {
        for (int x = 1; x < width - 1; x++)
        {
            int idx = y * width + x;
            float mag = gradientMagnitude[idx];
            float angle = gradientDirection[idx];

            // Normalize angle to [0, PI) and determine direction
            // Convert to degrees for easier comparison
            float angle_deg = angle * 180.0f / M_PI;
            if (angle_deg < 0)
            {
                angle_deg += 180.0f;
            }

            float neighbor1, neighbor2;

            // Determine which neighbors to compare based on gradient direction
            // Use linear interpolation for smoother edge detection
            // 0-22.5 or 157.5-180: horizontal (compare left/right)
            if ((angle_deg >= 0 && angle_deg < 22.5f) || (angle_deg >= 157.5f && angle_deg < 180.0f))
            {
                neighbor1 = gradientMagnitude[y * width + (x - 1)];
                neighbor2 = gradientMagnitude[y * width + (x + 1)];
            }
            // 22.5-67.5: diagonal (compare top-right/bottom-left)
            else if (angle_deg >= 22.5f && angle_deg < 67.5f)
            {
                // Use direct diagonal neighbors for cleaner edges
                neighbor1 = gradientMagnitude[(y - 1) * width + (x + 1)];
                neighbor2 = gradientMagnitude[(y + 1) * width + (x - 1)];
            }
            // 67.5-112.5: vertical (compare top/bottom)
            else if (angle_deg >= 67.5f && angle_deg < 112.5f)
            {
                neighbor1 = gradientMagnitude[(y - 1) * width + x];
                neighbor2 = gradientMagnitude[(y + 1) * width + x];
            }
            // 112.5-157.5: diagonal (compare top-left/bottom-right)
            else // angle_deg >= 112.5f && angle_deg < 157.5f
            {
                // Use direct diagonal neighbors for cleaner edges
                neighbor1 = gradientMagnitude[(y - 1) * width + (x - 1)];
                neighbor2 = gradientMagnitude[(y + 1) * width + (x + 1)];
            }

            // Keep pixel if it's a local maximum
            if (mag >= neighbor1 && mag >= neighbor2)
            {
                // Normalize magnitude to 0-255 range
                float normalized = mag * normalizeFactor;
                thinned[idx] = (unsigned char)std::max(0.0f, std::min(255.0f, normalized));
            }
            else
            {
                thinned[idx] = 0;
            }
        }
    }

    return thinned;
}

/**
 * Applies double threshold to classify pixels as strong, weak, or suppressed
 * 
 * @param thinned - Pointer to the thinned edge image
 * @param width - Width of the image in pixels
 * @param height - Height of the image in pixels
 * @param lowThreshold - Low threshold value (e.g., 0.05 * maxValue)
 * @param highThreshold - High threshold value (e.g., 0.15 * maxValue)
 * @return Pointer to newly allocated thresholded buffer, or NULL on failure
 */
unsigned char* DoubleThreshold(
    unsigned char* thinned,
    int width,
    int height,
    float lowThreshold,
    float highThreshold
)
{
    if (thinned == nullptr)
    {
        return nullptr;
    }

    unsigned char* thresholded = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    if (thresholded == nullptr)
    {
        return nullptr;
    }

    for (int i = 0; i < width * height; i++)
    {
        float value = (float)thinned[i];

        if (value >= highThreshold)
        {
            // Strong edge
            thresholded[i] = 255;
        }
        else if (value >= lowThreshold)
        {
            // Weak edge (tagged for potential connection)
            thresholded[i] = 50;
        }
        else
        {
            // Suppressed
            thresholded[i] = 0;
        }
    }

    return thresholded;
}

// Helper function for hysteresis edge tracing
static void traceEdge(unsigned char* edges, int width, int height, int x, int y)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
    {
        return;
    }

    int idx = y * width + x;

    // If this is a weak edge, check if it's connected to a strong edge
    if (edges[idx] == 50)
    {
        edges[idx] = 255; // Promote to strong

        // Check all 8 neighbors
        for (int dy = -1; dy <= 1; dy++)
        {
            for (int dx = -1; dx <= 1; dx++)
            {
                if (dx == 0 && dy == 0)
                {
                    continue;
                }

                int nx = x + dx;
                int ny = y + dy;

                if (nx >= 0 && nx < width && ny >= 0 && ny < height)
                {
                    int nidx = ny * width + nx;
                    if (edges[nidx] == 50)
                    {
                        traceEdge(edges, width, height, nx, ny);
                    }
                }
            }
        }
    }
}

/**
 * Performs hysteresis to connect weak edges to strong edges
 * 
 * @param thresholded - Pointer to the thresholded image (with values 0, 50, 255)
 * @param width - Width of the image in pixels
 * @param height - Height of the image in pixels
 * @return Pointer to newly allocated final edge buffer (binary: 0 or 255), or NULL on failure
 */
unsigned char* Hysteresis(
    unsigned char* thresholded,
    int width,
    int height
)
{
    if (thresholded == nullptr)
    {
        return nullptr;
    }

    unsigned char* edges = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    if (edges == nullptr)
    {
        return nullptr;
    }

    // Copy thresholded image
    memcpy(edges, thresholded, width * height * sizeof(unsigned char));

    // First pass: find all strong edges and trace from them
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = y * width + x;
            if (edges[idx] == 255)
            {
                // Trace from strong edges
                for (int dy = -1; dy <= 1; dy++)
                {
                    for (int dx = -1; dx <= 1; dx++)
                    {
                        if (dx == 0 && dy == 0)
                        {
                            continue;
                        }

                        int nx = x + dx;
                        int ny = y + dy;

                        if (nx >= 0 && nx < width && ny >= 0 && ny < height)
                        {
                            traceEdge(edges, width, height, nx, ny);
                        }
                    }
                }
            }
        }
    }

    // Final pass: set all remaining weak edges (not connected) to 0
    for (int i = 0; i < width * height; i++)
    {
        if (edges[i] == 50)
        {
            edges[i] = 0;
        }
    }

    return edges;
}

/**
 * Converts grayscale image to halftone using 2x2 block patterns
 * Each input pixel becomes a 2x2 block in the output
 * 
 * @param gray - Pointer to the grayscale image buffer (0-255)
 * @param width - Width of the input image in pixels
 * @param height - Height of the input image in pixels
 * @return Pointer to newly allocated halftone buffer (binary: 0 or 255), or NULL on failure
 */
unsigned char* Halftone_convert(unsigned char* gray, int width, int height)
{
    if (gray == nullptr || width <= 0 || height <= 0)
    {
        return nullptr;
    }

    // Output dimensions are 2x the input dimensions
    int newWidth = width * 2;
    int newHeight = height * 2;

    // Allocate output buffer
    unsigned char* halftone = (unsigned char*)malloc(newWidth * newHeight * sizeof(unsigned char));
    if (halftone == nullptr)
    {
        return nullptr;
    }

    // Initialize all pixels to black (0)
    memset(halftone, 0, newWidth * newHeight * sizeof(unsigned char));

    // Process each input pixel
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // Get input pixel value
            unsigned char pixel = gray[y * width + x];
            
            // Normalize to 0-1 range
            float i = (float)pixel / 255.0f;

            // Calculate output block position (2x2 block)
            int outX = x * 2;
            int outY = y * 2;

            // Determine which pixels in the 2x2 block should be white (255)
            // Block layout:
            // (0,0) top-left    (1,0) top-right
            // (0,1) bottom-left (1,1) bottom-right

            if (i >= 0.8f && i <= 1.0f)
            {
                // All 4 white
                halftone[outY * newWidth + outX] = 255;           // top-left
                halftone[outY * newWidth + (outX + 1)] = 255;    // top-right
                halftone[(outY + 1) * newWidth + outX] = 255;    // bottom-left
                halftone[(outY + 1) * newWidth + (outX + 1)] = 255; // bottom-right
            }
            else if (i >= 0.6f && i < 0.8f)
            {
                // bottom-left, top-right, bottom-right
                halftone[(outY + 1) * newWidth + outX] = 255;     // bottom-left
                halftone[outY * newWidth + (outX + 1)] = 255;    // top-right
                halftone[(outY + 1) * newWidth + (outX + 1)] = 255; // bottom-right
            }
            else if (i >= 0.4f && i < 0.6f)
            {
                // bottom-left, top-right
                halftone[(outY + 1) * newWidth + outX] = 255;    // bottom-left
                halftone[outY * newWidth + (outX + 1)] = 255;    // top-right
            }
            else if (i >= 0.2f && i < 0.4f)
            {
                // bottom-left only
                halftone[(outY + 1) * newWidth + outX] = 255;    // bottom-left
            }
            // else: 0 <= i < 0.2, all black (already initialized to 0)
        }
    }

    return halftone;
}

/**
 * Applies Floyd-Steinberg error diffusion dithering to reduce grayscale to 16 levels
 * 
 * @param gray - Pointer to the grayscale image buffer (0-255)
 * @param width - Width of the image in pixels
 * @param height - Height of the image in pixels
 * @return Pointer to newly allocated dithered buffer (16 levels: 0, 16, 32, ..., 240, 255), or NULL on failure
 */
unsigned char* FloydSteinberg_convert(unsigned char* gray, int width, int height)
{
    if (gray == nullptr || width <= 0 || height <= 0)
    {
        return nullptr;
    }

    // Allocate output buffer and copy input
    unsigned char* output = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    if (output == nullptr)
    {
        return nullptr;
    }

    // Copy input to output (we'll modify output in place)
    memcpy(output, gray, width * height * sizeof(unsigned char));

    // Process pixels in scanline order (top to bottom, left to right)
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = y * width + x;
            int oldValue = (int)output[idx];

            // Quantize to nearest of 16 levels
            int newValue = (int)(std::round(oldValue / 16.0f) * 16.0f);
            
            // Clamp to 0-255
            newValue = std::max(0, std::min(255, newValue));

            // Set quantized value
            output[idx] = (unsigned char)newValue;

            // Compute error
            int error = oldValue - newValue;

            // Distribute error to neighbors using Floyd-Steinberg kernel
            // Kernel weights: (x+1, y): 7/16, (x-1, y+1): 3/16, (x, y+1): 5/16, (x+1, y+1): 1/16
            
            // Check which neighbors are valid and collect their positions in Floyd-Steinberg order
            struct Neighbor {
                int x, y;
                float weight;
            };
            
            Neighbor neighbors[4];
            int validCount = 0;
            float weights[4] = {7.0f/16.0f, 3.0f/16.0f, 5.0f/16.0f, 1.0f/16.0f};

            // (x+1, y) - weight 7/16
            if (x + 1 < width)
            {
                neighbors[validCount] = {x + 1, y, weights[0]};
                validCount++;
            }

            // (x-1, y+1) - weight 3/16
            if (x - 1 >= 0 && y + 1 < height)
            {
                neighbors[validCount] = {x - 1, y + 1, weights[1]};
                validCount++;
            }

            // (x, y+1) - weight 5/16
            if (y + 1 < height)
            {
                neighbors[validCount] = {x, y + 1, weights[2]};
                validCount++;
            }

            // (x+1, y+1) - weight 1/16
            if (x + 1 < width && y + 1 < height)
            {
                neighbors[validCount] = {x + 1, y + 1, weights[3]};
                validCount++;
            }

            // Distribute error
            if (validCount > 0)
            {
                // Check if all 4 neighbors are present
                bool allNeighborsPresent = (validCount == 4);
                
                if (allNeighborsPresent)
                {
                    // Use standard Floyd-Steinberg weights (stored in neighbor struct)
                    for (int i = 0; i < 4; i++)
                    {
                        int errorAmount = (int)(error * neighbors[i].weight);
                        
                        int nIdx = neighbors[i].y * width + neighbors[i].x;
                        int newPixelValue = (int)output[nIdx] + errorAmount;
                        
                        // Clamp to 0-255
                        newPixelValue = std::max(0, std::min(255, newPixelValue));
                        output[nIdx] = (unsigned char)newPixelValue;
                    }
                }
                else
                {
                    // Split error equally among available neighbors
                    int errorPerNeighbor = error / validCount;
                    int remainder = error % validCount;
                    
                    for (int i = 0; i < validCount; i++)
                    {
                        // Distribute remainder to first few neighbors
                        int errorAmount = errorPerNeighbor + (i < remainder ? 1 : 0);
                        
                        int nIdx = neighbors[i].y * width + neighbors[i].x;
                        int newPixelValue = (int)output[nIdx] + errorAmount;
                        
                        // Clamp to 0-255
                        newPixelValue = std::max(0, std::min(255, newPixelValue));
                        output[nIdx] = (unsigned char)newPixelValue;
                    }
                }
            }
        }
    }

    return output;
}

/**
 * Creates a composite image with 4 results in a 2x2 grid:
 * Top-left: Grayscale, Top-right: Canny, Bottom-left: Halftone, Bottom-right: Floyd-Steinberg
 * 
 * @param grayscale - Pointer to grayscale image (width x height)
 * @param canny - Pointer to Canny edge image (width x height)
 * @param halftone - Pointer to Halftone image (width*2 x height*2)
 * @param floydsteinberg - Pointer to Floyd-Steinberg image (width x height)
 * @param width - Width of grayscale/canny/floydsteinberg images
 * @param height - Height of grayscale/canny/floydsteinberg images
 * @return Pointer to newly allocated composite buffer (width*2 x height*2), or NULL on failure
 */
unsigned char* CreateComposite(
    unsigned char* grayscale,
    unsigned char* canny,
    unsigned char* halftone,
    unsigned char* floydsteinberg,
    int width,
    int height
)
{
    if (grayscale == nullptr || canny == nullptr || halftone == nullptr || floydsteinberg == nullptr)
    {
        return nullptr;
    }

    int compositeWidth = width * 2;
    int compositeHeight = height * 2;

    // Allocate composite buffer
    unsigned char* composite = (unsigned char*)malloc(compositeWidth * compositeHeight * sizeof(unsigned char));
    if (composite == nullptr)
    {
        return nullptr;
    }

    // Initialize to black
    memset(composite, 0, compositeWidth * compositeHeight * sizeof(unsigned char));

    // Top-left: Grayscale
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            composite[y * compositeWidth + x] = grayscale[y * width + x];
        }
    }

    // Top-right: Canny
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            composite[y * compositeWidth + (width + x)] = canny[y * width + x];
        }
    }

    // Bottom-left: Halftone (scale down from 2x size by averaging 2x2 blocks)
    int halftoneWidth = width * 2;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // Average the 2x2 block from halftone to get a single pixel value
            int halftoneX = x * 2;
            int halftoneY = y * 2;
            
            // Get all 4 pixels in the 2x2 block
            int sum = 0;
            sum += halftone[halftoneY * halftoneWidth + halftoneX];           // top-left
            sum += halftone[halftoneY * halftoneWidth + (halftoneX + 1)];     // top-right
            sum += halftone[(halftoneY + 1) * halftoneWidth + halftoneX];    // bottom-left
            sum += halftone[(halftoneY + 1) * halftoneWidth + (halftoneX + 1)]; // bottom-right
            
            // Average and set in composite
            unsigned char avg = (unsigned char)(sum / 4);
            composite[(height + y) * compositeWidth + x] = avg;
        }
    }

    // Bottom-right: Floyd-Steinberg
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            composite[(height + y) * compositeWidth + (width + x)] = floydsteinberg[y * width + x];
        }
    }

    return composite;
}

/**
 * Complete Canny edge detection pipeline wrapper
 * 
 * @param gray - Pointer to the grayscale image buffer
 * @param width - Width of the image in pixels
 * @param height - Height of the image in pixels
 * @return Pointer to newly allocated edge buffer (binary: 0 or 255), or NULL on failure
 */
unsigned char* Canny_convert(
    unsigned char* gray,
    int width,
    int height
)
{
    if (gray == nullptr || width <= 0 || height <= 0)
    {
        return nullptr;
    }

    // Step 1: Gaussian Blur
    unsigned char* blurred = GaussianBlur(gray, width, height);
    if (blurred == nullptr)
    {
        return nullptr;
    }

    // Step 2: Gradient calculation
    float* gradientMagnitude = (float*)malloc(width * height * sizeof(float));
    float* gradientDirection = (float*)malloc(width * height * sizeof(float));
    
    if (gradientMagnitude == nullptr || gradientDirection == nullptr)
    {
        free(blurred);
        if (gradientMagnitude) free(gradientMagnitude);
        if (gradientDirection) free(gradientDirection);
        return nullptr;
    }

    Gradient(blurred, width, height, gradientMagnitude, gradientDirection);

    // Step 3: Non-maximum suppression
    unsigned char* thinned = NonMaxSuppression(gradientMagnitude, gradientDirection, width, height);
    if (thinned == nullptr)
    {
        free(blurred);
        free(gradientMagnitude);
        free(gradientDirection);
        return nullptr;
    }

    // Find maximum value for threshold calculation
    float maxValue = 0.0f;
    for (int i = 0; i < width * height; i++)
    {
        if (gradientMagnitude[i] > maxValue)
        {
            maxValue = gradientMagnitude[i];
        }
    }

    // Step 4: Double threshold
    // Increased thresholds to filter out more edges (fewer white pixels)
    float lowThreshold = 0.20f * maxValue;   // Increased low threshold
    float highThreshold = 0.45f * maxValue;   // Increased high threshold
    
    unsigned char* thresholded = DoubleThreshold(thinned, width, height, lowThreshold, highThreshold);
    if (thresholded == nullptr)
    {
        free(blurred);
        free(gradientMagnitude);
        free(gradientDirection);
        free(thinned);
        return nullptr;
    }

    // Step 5: Hysteresis
    unsigned char* edges = Hysteresis(thresholded, width, height);

    // Cleanup intermediate buffers
    free(blurred);
    free(gradientMagnitude);
    free(gradientDirection);
    free(thinned);
    free(thresholded);

    return edges;
}

int main(void)
{
    // Load the input image
    std::string input_filepath = "res/textures/Lenna.png";
    int width, height, channels;
    
    // Load image with desired_channels = 0 to get original channels, or 4 to force RGBA
    unsigned char* input_image = stbi_load(input_filepath.c_str(), &width, &height, &channels, 0);
    
    if (input_image == nullptr)
    {
        std::cerr << "Error: Could not load image " << input_filepath << std::endl;
        std::cerr << "Reason: " << stbi_failure_reason() << std::endl;
        return 1;
    }

    std::cout << "Image loaded successfully!" << std::endl;
    std::cout << "Width: " << width << ", Height: " << height << ", Channels: " << channels << std::endl;

    // Convert to grayscale
    unsigned char* grayscale_image = Grayscale_convert(input_image, width, height, channels);
    
    if (grayscale_image == nullptr)
    {
        std::cerr << "Error: Grayscale conversion failed!" << std::endl;
        stbi_image_free(input_image);
        return 1;
    }

    std::cout << "Grayscale conversion completed!" << std::endl;

    // Save the grayscale image
    std::string output_filepath = "res/images/Grayscale.png";
    int result = stbi_write_png(output_filepath.c_str(), width, height, 1, grayscale_image, width * 1);
    
    if (result == 0)
    {
        std::cerr << "Error: Could not save grayscale image!" << std::endl;
        stbi_image_free(input_image);
        free(grayscale_image);
        return 1;
    }

    std::cout << "Grayscale image saved as " << output_filepath << std::endl;

    // Write pixel values (0-15) to text file
    std::string txt_filepath = "res/textFiles/Grayscale.txt";
    std::ofstream txt_file(txt_filepath);
    
    if (!txt_file.is_open())
    {
        std::cerr << "Error: Could not create text file " << txt_filepath << std::endl;
        stbi_image_free(input_image);
        free(grayscale_image);
        return 1;
    }

    // Write pixel values quantized to 0-15 range, separated by commas (no spaces)
    for (int i = 0; i < width * height; i++)
    {
        // Quantize from 0-255 to 0-15 (divide by 16)
        unsigned char quantized = grayscale_image[i] / 16;
        
        txt_file << (int)quantized;
        
        // Add comma after each number except the last one
        if (i < width * height - 1)
        {
            txt_file << ",";
        }
    }
    
    txt_file.close();
    std::cout << "Pixel values saved to " << txt_filepath << std::endl;

    // Apply Halftone conversion
    std::cout << "Starting Halftone conversion..." << std::endl;
    unsigned char* halftone_image = Halftone_convert(grayscale_image, width, height);
    
    if (halftone_image == nullptr)
    {
        std::cerr << "Error: Halftone conversion failed!" << std::endl;
        stbi_image_free(input_image);
        free(grayscale_image);
        return 1;
    }

    std::cout << "Halftone conversion completed!" << std::endl;

    // Save Halftone image
    int halftone_width = width * 2;
    int halftone_height = height * 2;
    std::string halftone_filepath = "res/images/Halftone.png";
    int halftone_result = stbi_write_png(halftone_filepath.c_str(), halftone_width, halftone_height, 1, halftone_image, halftone_width);
    
    if (halftone_result == 0)
    {
        std::cerr << "Error: Could not save Halftone image!" << std::endl;
        stbi_image_free(input_image);
        free(grayscale_image);
        free(halftone_image);
        return 1;
    }

    std::cout << "Halftone image saved as " << halftone_filepath << std::endl;
    std::cout << "Halftone dimensions: " << halftone_width << "x" << halftone_height << std::endl;

    // Apply Floyd-Steinberg error diffusion dithering
    std::cout << "Starting Floyd-Steinberg dithering..." << std::endl;
    unsigned char* floydsteinberg_image = FloydSteinberg_convert(grayscale_image, width, height);
    
    if (floydsteinberg_image == nullptr)
    {
        std::cerr << "Error: Floyd-Steinberg dithering failed!" << std::endl;
        stbi_image_free(input_image);
        free(grayscale_image);
        free(halftone_image);
        return 1;
    }

    std::cout << "Floyd-Steinberg dithering completed!" << std::endl;

    // Save Floyd-Steinberg image
    std::string floydsteinberg_filepath = "res/images/FloyedSteinberg.png";
    int floydsteinberg_result = stbi_write_png(floydsteinberg_filepath.c_str(), width, height, 1, floydsteinberg_image, width);
    
    if (floydsteinberg_result == 0)
    {
        std::cerr << "Error: Could not save Floyd-Steinberg image!" << std::endl;
        stbi_image_free(input_image);
        free(grayscale_image);
        free(halftone_image);
        free(floydsteinberg_image);
        return 1;
    }

    std::cout << "Floyd-Steinberg image saved as " << floydsteinberg_filepath << std::endl;

    // Apply Canny edge detection
    std::cout << "Starting Canny edge detection..." << std::endl;
    unsigned char* canny_edges = Canny_convert(grayscale_image, width, height);
    
    if (canny_edges == nullptr)
    {
        std::cerr << "Error: Canny edge detection failed!" << std::endl;
        stbi_image_free(input_image);
        free(grayscale_image);
        return 1;
    }

    std::cout << "Canny edge detection completed!" << std::endl;

    // Save Canny edge image
    std::string canny_filepath = "res/images/Canny.png";
    int canny_result = stbi_write_png(canny_filepath.c_str(), width, height, 1, canny_edges, width);
    
    if (canny_result == 0)
    {
        std::cerr << "Error: Could not save Canny edge image!" << std::endl;
        stbi_image_free(input_image);
        free(grayscale_image);
        free(canny_edges);
        return 1;
    }

    std::cout << "Canny edge image saved as " << canny_filepath << std::endl;

    // Write Canny pixel values (0-1) to text file
    std::string canny_txt_filepath = "res/textFiles/Canny.txt";
    std::ofstream canny_txt_file(canny_txt_filepath);
    
    if (!canny_txt_file.is_open())
    {
        std::cerr << "Error: Could not create Canny text file " << canny_txt_filepath << std::endl;
        stbi_image_free(input_image);
        free(grayscale_image);
        free(canny_edges);
        return 1;
    }

    // Write pixel values as 0 or 1, separated by commas (no spaces)
    // 255 (white/edge) -> 1, 0 (black/no edge) -> 0
    for (int i = 0; i < width * height; i++)
    {
        // Convert 255 to 1, 0 stays 0
        int value = (canny_edges[i] == 255) ? 1 : 0;
        
        canny_txt_file << value;
        
        // Add comma after each number except the last one
        if (i < width * height - 1)
        {
            canny_txt_file << ",";
        }
    }
    
    canny_txt_file.close();
    std::cout << "Canny pixel values (0-1) saved to " << canny_txt_filepath << std::endl;

    // Create composite image (2x2 grid)
    std::cout << "Creating composite image..." << std::endl;
    unsigned char* composite_image = CreateComposite(
        grayscale_image,
        canny_edges,
        halftone_image,
        floydsteinberg_image,
        width,
        height
    );

    if (composite_image == nullptr)
    {
        std::cerr << "Error: Composite image creation failed!" << std::endl;
        stbi_image_free(input_image);
        free(grayscale_image);
        free(halftone_image);
        free(floydsteinberg_image);
        free(canny_edges);
        return 1;
    }

    // Save composite image
    int composite_width = width * 2;
    int composite_height = height * 2;
    std::string composite_filepath = "res/images/Composite.png";
    int composite_result = stbi_write_png(composite_filepath.c_str(), composite_width, composite_height, 1, composite_image, composite_width);

    if (composite_result == 0)
    {
        std::cerr << "Error: Could not save composite image!" << std::endl;
        stbi_image_free(input_image);
        free(grayscale_image);
        free(halftone_image);
        free(floydsteinberg_image);
        free(canny_edges);
        free(composite_image);
        return 1;
    }

    std::cout << "Composite image saved as " << composite_filepath << std::endl;
    std::cout << "Composite dimensions: " << composite_width << "x" << composite_height << std::endl;

    // Free allocated memory
    stbi_image_free(input_image);
    free(grayscale_image);
    free(halftone_image);
    free(floydsteinberg_image);
    free(canny_edges);
    free(composite_image);

    return 0;
}

