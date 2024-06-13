package main

import (
	"bufio"
	"fmt"
	"io"
	"os"
	"strconv"
	"strings"
	"sync"
)

func main() {
	// debug.SetGCPercent(-1)

	inputFile, _ := os.Open("build/gradient.pgm")
	defer inputFile.Close()

	reader := bufio.NewReader(inputFile)

	outputFile, _ := os.Create("build/inverted_gradient_go.pgm")
	defer outputFile.Close()

	width := 0
	height := 0
	writer := bufio.NewWriter(outputFile)
	defer writer.Flush()

	// Write the PGM header to the output file
	for i := 0; i < 3; i++ {
		line, _ := reader.ReadString('\n')
		if i == 1 {
			parts := strings.Fields(line)
			width, _ = strconv.Atoi(parts[0])
			height, _ = strconv.Atoi(parts[1])
		}
		fmt.Fprint(writer, line)
	}

	var wg sync.WaitGroup

	pixels, _ := io.ReadAll(reader)

	totalPixels := height * width
	for index := 0; index < totalPixels; index++ {
		wg.Add(1)

		go func(i int) {
			pixels[i] = 255 - pixels[i]
			wg.Done()
		}(index)
	}

	wg.Wait()

	writer.Write(pixels)

	fmt.Println("New image saved as inverted_gradient_go.pgm")
}
