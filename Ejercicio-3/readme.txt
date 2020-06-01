Compilar:
	- make
Correr:
	mpirun -np [cantProc] ./build/ejercicio3A [tam. matriz]	
	mpirun -np [cantProc] ./build/ejercicio3B [tam. matriz]

Otra forma:
	- make 3A np=16 size=6
	- make 3B np=16 size=6
	


