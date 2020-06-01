Inicio de programa
    
    variable MatrizJuego

    For t in time 
        for celula in matrizJuego
            Procesar submatriz(submatriz celulas)
        end for
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
