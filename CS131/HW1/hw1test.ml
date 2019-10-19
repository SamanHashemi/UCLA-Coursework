let my_subset_test0 = subset [1;2;2;1;1;2;2;1] [2;1]
let my_subset_test1 = subset [] []
let my_subset_test2 = not ( subset [4; 6; 2] [3; 4; 5] )

let my_equal_sets_test0 = equal_sets [0;1;2;3] [3;2;1;0]
let my_equal_sets_test1 = not (equal_sets [1;2;3] [])

let my_set_union_test0 = equal_sets (set_union [1] [2]) [1;2]
let my_set_union_test1 = equal_sets (set_union ["a"; "b"] ["c"; "d"]) ["a"; "b"; "c"; "d"]
let my_set_union_test2 = equal_sets (set_union [1;1] [2;2]) [1;2]

let my_set_intersection_test0 = equal_sets (set_intersection [900;800;700;600;500;100] [1;2;3;4;5;6;7;8;9]) [] 
let my_set_intersection_test1 = equal_sets (set_intersection [1;2;3;4;5;6;7;8] [8;7;6;5]) [8;7;6;5]
let my_set_intersection_test2 = equal_sets (set_intersection [1;1;1;1;1;1;1;2] [2]) [2]

let my_set_diff_test0 = equal_sets (set_diff [1;2;3;4;5] [1;2;3;4;6;7]) [5]
let my_set_diff_test1 = equal_sets (set_diff [1] [1;4;5;6;7]) []
let my_set_diff_test2 = equal_sets (set_diff [1;12] [1;4;5;6;7]) [12]

let my_computed_fixed_point_test0 = computed_fixed_point (=) (fun x -> x) 10 = 10
let my_computed_fixed_point_test1 = computed_fixed_point (=) (fun x -> x/3) 3333 = 0
let my_computed_fixed_point_test1 = computed_fixed_point (=) (fun x -> x / 3) 301 = 0

type nonterminals =
  | A | B | C | D | E | F


let rules =
  [   (A, [N B; N C; N D; N E; N F]);
    (B, [N C; N D; N E; N F]);
    (C, [N D; N E; N F]);
    (D, [N E; N F]);
    (E, [N F])  ]


let b = (B, [(B, [N C; N D; N E; N F]); (C, [N D; N E; N F]); (D, [N E; N F]); (E, [N F])])
let c = (C, [(C, [N D; N E; N F]); (D, [N E; N F]); (E, [N F])])
let d = (D, [(D, [N E; N F]); (E, [N F])])
let e = (E, [(E, [N F])])

let my_filter_reachable_test0 = filter_reachable (B, rules) = b
let my_filter_reachable_test1 = filter_reachable (C, rules) = c
let my_filter_reachable_test2 = filter_reachable (D, rules) = d
let my_filter_reachable_test3 = filter_reachable (E, rules) = e