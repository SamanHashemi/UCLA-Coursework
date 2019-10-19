fun everyNth(L: List<Any>,N: Int ) : List<Any>{
    var count : Int = N
    var newL : MutableList<Any> = arrayListOf()
    if(count <= 0){ 
        return newL 
    }
    while(count <= L.size){
        newL.add(L.get(count-1))
        count += N;
    }
    return newL.toList()
}