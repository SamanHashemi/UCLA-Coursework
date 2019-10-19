fun main(args: Array<String>) {
    println("Printing List for every 3rd element:")
    val l = listOf("1st", "2nd", "3rd", "4th", "5th", "6th", "7th")
    var newL : List<Any> = everyNth(l, 3)
    for (i in newL){
        println(i)
    }
    println()
    println("Printing List for every 7th element:")
    var newL1 : List<Any> = everyNth(l, 7)
    for (i in newL1){
        println(i)
    }
    println()
    println("Printing List for every 30th element (no result):")
    var newL2 : List<Any> = everyNth(l, 30)
    for (i in newL2){
        println(i)
    }
}