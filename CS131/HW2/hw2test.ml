
type web_non_terminals = 
| Exp | Tot | Num | Op

let accept_all string = Some string

let accept_empty_suffix = function
   | _::_ -> None
   | x -> Some x

let mathgram =
	(Exp, function
	| Exp -> [[N Num; N Op; N Num]; [N Num; N Op; N Tot]]
	| Tot -> [[N Num]; [N Num; N Op; N Tot];[N Op; N Tot] ]
	| Num -> [[T"1"];[T"2"];[T"3"];[T"4"];]
	| Op -> [[T"+"];[T"-"];[T"*"];[T"/"]])

let wrong_frag = ["+"; "-"; "1"; "2"]
let right_frag = ["1"; "+"; "2"]


let make_matcher_test = ((make_matcher mathgram accept_all wrong_frag) = None)
let make_matcher_test2 = ((make_matcher mathgram accept_all right_frag) = Some [])

let make_parser_test = ((make_parser mathgram wrong_frag) = None)
let make_parser_test2 =  (match make_parser mathgram right_frag with
							| Some x -> (right_frag = parse_tree_leaves x)
							| None -> false)