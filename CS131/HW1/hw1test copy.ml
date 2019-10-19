let my_subset_test0 = subset [] []
let my_subset_test1 = subset [] [5;7;11;13]
let my_subset_test2 = subset [4;2;5;7] [7;5;4;2]
let my_subset_test3 = not (subset [4;5;3] [8;7;4;9;2])
let my_subset_test4 = subset [2;2;2;2] [2]

let my_equal_sets_test0 = equal_sets [] []
let my_equal_sets_test1 = equal_sets [3;3;3] [3]
let my_equal_sets_test2 = not (equal_sets [] [1])
let my_equal_sets_test3 = not (equal_sets [2;3;4] [1;2;3])

let my_set_union_test0 = equal_sets (set_union [1;2;3;4] []) [1;2;3;4]
let my_set_union_test1 = equal_sets (set_union [1;2;3;4] [3;1;3]) [1;2;3;4]
let my_set_union_test2 = equal_sets (set_union [3;3;3;3] []) [3]
let my_set_union_test3 = equal_sets (set_union [] [4;4;1]) [1;4]
let my_set_union_test4 = not (equal_sets (set_union [] [4;2;1]) [1;4])

let my_set_intersection_test0 =
  equal_sets (set_intersection [7;5;6] []) []
let my_set_intersection_test1 =
  equal_sets (set_intersection [4;2;3;4] [3;1;3;2]) [2;3]
let my_set_intersection_test2 =
  equal_sets (set_intersection [3;2;1] [1;1;3;2]) [1;2;3]
let my_set_intersection_test3 =
  equal_sets (set_intersection [3;3;3;3;3] [3; 3]) [3]
let my_set_intersection_test4 =
  equal_sets (set_intersection [1;1;1;2;3;2;1] [1]) [1]
let my_set_intersection_test5 =
  equal_sets (set_intersection [1] [1;1;1;1;1]) [1]
let my_set_intersection_test6 =
  equal_sets (set_intersection ["c"] ["a"; "a"; "b"; "a"; "c"]) ["c"]


let my_set_diff_test0 = equal_sets (set_diff [5;6;7] [6;4;3;8;9]) [5;7]
let my_set_diff_test1 = equal_sets (set_diff [] []) []
let my_set_diff_test2 = equal_sets (set_diff [] [2;3;4;3]) []
let my_set_diff_test3 = equal_sets (set_diff [2;3;4;3] []) [2;3;4]
let my_set_diff_test4 = equal_sets (set_diff ["a"; "b"; "c"; "d"] ["a"; "a"; "b"; "b"]) ["c"; "d"]

let my_computed_fixed_point_test0 =
  computed_fixed_point (=) (fun x -> x mod 3) 99 = 0
let my_computed_fixed_point_test1 =
  computed_fixed_point (=) (fun x -> x *. 5.) 3. = infinity
let my_computed_periodic_point_test3 =
  computed_periodic_point (=) (fun x -> -x) 2 (-1) = -1

let my_while_away_test0 = 
  equal_sets (while_away ((+) 5) ((>) 24) 0) [0; 5; 10; 15; 20]
let my_while_away_test1 = 
  equal_sets (while_away ((+) 8) ((<) 13) 0) []

let my_rle_decode_test0 = 
  equal_sets (rle_decode [3,1; 1,2]) [1; 1; 1; 2]
let my_rle_decode_test1 = 
  equal_sets (rle_decode [2,"a"; 0,"b"; 1,"c"; 3,"z"]) ["a"; "a"; "c"; "z"; "z"; "z"]
let my_rle_decode_test2 =
  equal_sets (rle_decode [2, 2.; 2, 0.0; 0, 3.0]) [2.; 2.; 0.0; 0.0]

type nonterminals =
  | Expr | Class | Test | Grade

let rules =
   [Expr, [T"("; N Expr; T")"];
    Expr, [N Class];
    Expr, [N Expr; N Class; N Expr];
    Class, [N Class];
    Class, [N Expr; T"Assignments"; N Expr];
    Class, [N Expr; T"Help"];
    Test, [T"Failed"];
    Test, [T"Barely passed"];
    Grade, []]

let g = Expr, rules

let my_filter_blind_alleys_test0 = filter_blind_alleys g
    = (Expr, [Test, [T"Failed"];
       Test, [T"Barely passed"];
       Grade, []])

let my_filter_blind_alleys_test1 =
  filter_blind_alleys (Expr,
      [Expr, [T"("; N Expr; T")"];
       Expr, [N Class];
       Expr, [N Expr; N Class; N Expr];
       Class, [N Class];
       Class, [N Expr; T"Assignments"; N Expr];
       Class, [N Expr; T"Help"];
       Class, [N Test];
       Test, [T"Failed"];
       Test, [T"Barely passed"];
       Grade, [N Expr];
       Grade, [T"D"; N Grade];
       Grade, [N Class]])
  = (Expr,
      [Expr, [T"("; N Expr; T")"];
       Expr, [N Class];
       Expr, [N Expr; N Class; N Expr];
       Class, [N Class];
       Class, [N Expr; T"Assignments"; N Expr];
       Class, [N Expr; T"Help"];
       Class, [N Test];
       Test, [T"Failed"];
       Test, [T"Barely passed"];
       Grade, [N Expr];
       Grade, [T"D"; N Grade];
       Grade, [N Class]])
