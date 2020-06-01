Inicio de programa
    
    variable MatrizJuego

    For t in time 
        Divido en submatrices MatrizJuego()
        Procesar submatriz(submatriz celulas)
        Junto los resultados()
        Rearmo la matriz()
        Seteo nuevo estado()
    end for

Fin de programa

Procesar submatriz(submatriz celulas){
    
    submatrizNew 

    for i in celulas
        celula = celula.nextState(vecinos)
    
    return submatrizNew
}
