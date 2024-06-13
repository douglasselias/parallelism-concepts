package main

import (
	"bufio"
	"fmt"
	"io"
	"os"
	"runtime"
	"strconv"
	"strings"
	"sync"
)

type Image struct {
	pixels []byte
	width  int
	height int
}

type GoArgs struct {
	image    Image
	startRow int
	endRow   int
}

func main() {
	// debug.SetGCPercent(-1)

	inputFile, _ := os.Open("build/gradient.pgm")
	defer inputFile.Close()

	reader := bufio.NewReader(inputFile)

	outputFile, _ := os.Create("build/inverted_gradient_go_chunks.pgm")
	defer outputFile.Close()

	image := Image{}
	writer := bufio.NewWriter(outputFile)
	defer writer.Flush()

	// Write the PGM header to the output file
	for i := 0; i < 3; i++ {
		line, _ := reader.ReadString('\n')
		if i == 1 {
			parts := strings.Fields(line)
			image.width, _ = strconv.Atoi(parts[0])
			image.height, _ = strconv.Atoi(parts[1])
		}
		fmt.Fprint(writer, line)
	}

	var wg sync.WaitGroup

	pixels, _ := io.ReadAll(reader)
	image.pixels = pixels

	totalThreads := runtime.NumCPU() - 1
	rowsPerThread := image.height / totalThreads
	extraRows := image.height % totalThreads
	startRow := 0

	for i := 0; i < totalThreads; i++ {
		wg.Add(1)

		countExtraRows := 0
		if i < extraRows {
			countExtraRows = 1
		}

		args := GoArgs{
			image:    image,
			startRow: startRow,
			endRow:   startRow + rowsPerThread + countExtraRows,
		}
		startRow = args.endRow

		go func(args GoArgs) {
			for row := args.startRow; row < args.endRow; row++ {
				for col := 0; col < args.image.width; col++ {
					index := row*args.image.width + col
					args.image.pixels[index] = 255 - args.image.pixels[index]
				}
			}
			wg.Done()
		}(args)
	}

	wg.Wait()

	writer.Write(pixels)

	fmt.Println("New image saved as inverted_gradient_go_chunks.pgm")
}
