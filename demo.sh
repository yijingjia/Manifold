mkdir build
cd build
cmake ..
make
./manifold ../examples/input.obj ../examples/output.obj
./manifold ../examples/FinalBaseMesh.obj ../examples/FinalBaseMeshOutput.obj
./manifold ../examples/bed.obj ../examples/bedOutput.obj
./manifold ../examples/bathtub.obj ../examples/bathtubOutput.obj
./manifold ../examples/chair.off ../examples/chairOutput.off
./manifold ../examples/table.off ../examples/tableOutput.off


#./simplify -i ../examples/manifold.obj -o ../examples/simplified_manifold.obj -c 1e-2 -f 10000 -r 0.2
cd ..

