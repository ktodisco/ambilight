dbg:
	(cd led; make)
	(cd analyzer; make)
	(cd ambilight; make)
	(cd test_ambilight; make)
	(cd test_led; make)
	(cd test_video; make)

rel:
	(cd led; make rel)
	(cd analyzer; make rel)
	(cd ambilight; make rel)
	(cd test_ambilight; make rel)
	(cd test_led; make rel)
	(cd test_video; make rel)

clean:
	(cd led; make clean)
	(cd analyzer; make clean)
	(cd ambilight; make clean)
	(cd test_ambilight; make clean)
	(cd test_led; make clean)
	(cd test_video; make clean)
