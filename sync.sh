#!/bin/bash
rsync -zarv --exclude="*.vtk" --exclude="build" --exclude="run" ./ wolf:/export/home/ruslan/SOFTWARE/gpuPacking
