const fs = require('fs');

const inputFile = 'build/gradient.pgm';
const outputFile = 'build/inverted_gradient_nodejs.pgm';

fs.readFile(inputFile, (err, data) => {
  // Find the end of the header (after '255\n')
  const headerEndIndex = data.indexOf('\n', data.indexOf('\n', data.indexOf('\n') + 1) + 1) + 1;
  const header = data.toString('ascii', 0, headerEndIndex);
  const pixelsBuffer = Buffer.from(data.slice(headerEndIndex));

  for (let i = 0; i < pixelsBuffer.length; i++) {
    pixelsBuffer[i] = 255 - pixelsBuffer[i];
  }

  fs.writeFile(outputFile, Buffer.concat([Buffer.from(header), pixelsBuffer]), () => {
    console.log(`New image saved as ${outputFile}`);
  });
});
