#! /bin/tcsh
rm -rf KB/*
rm -rf output/*
foreach train (`ls data`)
	bin/training $train
  bin/recall $train
end
