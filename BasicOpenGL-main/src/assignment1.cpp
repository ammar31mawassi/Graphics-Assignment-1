#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include <iostream>
#include <cstdlib>
#include <algorithm>

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
    std::string output_filepath = "res/textures/Lenna_grayscale.png";
    int result = stbi_write_png(output_filepath.c_str(), width, height, 1, grayscale_image, width * 1);
    
    if (result == 0)
    {
        std::cerr << "Error: Could not save grayscale image!" << std::endl;
        stbi_image_free(input_image);
        free(grayscale_image);
        return 1;
    }

    std::cout << "Grayscale image saved as " << output_filepath << std::endl;

    // Free allocated memory
    stbi_image_free(input_image);
    free(grayscale_image);

    return 0;
}

