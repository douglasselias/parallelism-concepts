input_file = 'build/gradient.pgm'
output_file = 'build/inverted_gradient_python.pgm'

with open(input_file, 'rb') as f:
    header = []
    for _ in range(3):
        header.append(f.readline())
    
    width, height = map(int, header[1].split())
    pixels = bytearray(f.read())
    
inverted_pixels = bytearray(255 - pixel for pixel in pixels)

with open(output_file, 'wb') as f:
    f.writelines(header)
    f.write(inverted_pixels)

print(f"New image saved as {output_file}")
