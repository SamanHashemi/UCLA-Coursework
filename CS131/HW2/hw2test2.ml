type web_non_terminals = 
| Website | Name | Alnum | Tail

let accept_empty_suffix = function
  | _::_ -> None
  | x -> Some x

let accept_all string = Some string

let website_grammar =
  (Website, 
  function
  | Website -> [[T"www"; T"."; N Name; T"."; N Tail];
          [N Name; T"."; N Tail]]
  | Name -> [[N Alnum];
         [N Alnum; N Name]]
  | Alnum -> [[T"0"];
        [T"1"];
        [T"2"];
        [T"3"];
        [T"4"];
        [T"5"];
        [T"6"];
        [T"7"];
        [T"8"];
        [T"9"];
        [T"a"];
        [T"b"];
        [T"c"];
        [T"d"];
        [T"e"];
        [T"f"];
        [T"g"];
        [T"h"];
        [T"i"];
        [T"j"];
        [T"k"];
        [T"l"];
        [T"m"];
        [T"n"];
        [T"o"];
        [T"p"];
        [T"q"];
        [T"r"];
        [T"s"];
        [T"t"];
        [T"u"];
        [T"v"];
        [T"w"];
        [T"x"];
        [T"y"];
        [T"z"];]
  | Tail -> [[T"com"];
         [T"edu"];
         [T"net"]])

let google_frag = ["www"; "."; "g"; "o"; "o"; "g"; "l"; "e"; "."; "com"]
let invalid_frag = ["www"; "www"; "."; "com";]

let make_matcher_test = (((make_matcher website_grammar accept_all google_frag) = Some [])
            && (make_matcher website_grammar accept_all invalid_frag) = None)

let make_parser_test = (((make_parser website_grammar invalid_frag) = None)
  && (match make_parser website_grammar google_frag with
    | None -> false
    | Some x -> (google_frag = parse_tree_leaves x)))
