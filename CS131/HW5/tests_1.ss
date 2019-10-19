(define a "a") (define b "b") (define c "c") (define d "d") ; So we can evaluate test cases

(printf "~v~n" (expr-compare 12 12))
(printf "~v~n" "12")
(printf "~v~n~n" (test-expr-compare 12 12))

(printf "~v~n" (expr-compare 12 20))
(printf "~v~n" "(if % 12 20)")
(printf "~v~n~n" (test-expr-compare 12 20))

(printf "~v~n" (expr-compare #t #t))
(printf "~v~n" "#t")
(printf "~v~n~n" (test-expr-compare #t #t))

(printf "~v~n" (expr-compare #f #f))
(printf "~v~n" "#f")
(printf "~v~n~n" (test-expr-compare #f #f))

(printf "~v~n" (expr-compare #t #f))
(printf "~v~n" "%")
(printf "~v~n~n" (test-expr-compare #t #f))

(printf "~v~n" (expr-compare #f #t))
(printf "~v~n" "(not %)")
(printf "~v~n~n" (test-expr-compare #f #t))

(printf "~v~n" (expr-compare 'a '(cons a b)))
(printf "~v~n" "(if % a (cons a b))")
(printf "~v~n~n" (test-expr-compare 'a '(cons a b)))

(printf "~v~n" (expr-compare '(cons a b) '(cons a b)))
(printf "~v~n" "(cons a b)")
(printf "~v~n~n" (test-expr-compare '(cons a b) '(cons a b)))

(printf "~v~n" (expr-compare '(cons a b) '(cons a c)))
(printf "~v~n" "(cons a (if % b c))")
(printf "~v~n~n" (test-expr-compare '(cons a b) '(cons a c)))

(printf "~v~n" (expr-compare '(cons (cons a b) (cons b c))
              '(cons (cons a c) (cons a c))))
(printf "~v~n" "(cons (cons a (if % b c)) (cons (if % b a) c))")
(printf "~v~n~n" (test-expr-compare '(cons (cons a b) (cons b c))
              '(cons (cons a c) (cons a c))))

(printf "~v~n" (expr-compare '(cons a b) '(list a b)))
(printf "~v~n" "((if % cons list) a b)")
(printf "~v~n~n" (test-expr-compare '(cons a b) '(list a b)))

(printf "~v~n" (expr-compare '(list) '(list a)))
(printf "~v~n" "(if % (list) (list a))")
(printf "~v~n~n" (test-expr-compare '(list) '(list a)))

(printf "~v~n" (expr-compare ''(a b) ''(a c)))
(printf "~v~n" "(if % '(a b) '(a c))")
(printf "~v~n~n" (test-expr-compare ''(a b) ''(a c)))

(printf "~v~n" (expr-compare '(quote (a b)) '(quote (a c))))
(printf "~v~n" "(if % '(a b) '(a c))")
(printf "~v~n~n" (test-expr-compare '(quote (a b)) '(quote (a c))))

(printf "~v~n" (expr-compare '(quoth (a b)) '(quoth (a c))))
(printf "~v~n~n" "(quoth (a (if % b c)))")
; (printf "~v~n~n" (test-expr-compare '(quoth (a b)) '(quoth (a c)))) ; Broken

(printf "~v~n" (expr-compare '(if x y z) '(if x z z)))
(printf "~v~n~n" "(if x (if % y z) z)")
; (printf "~v~n~n" (test-expr-compare '(if x y z) '(if x z z)))

(printf "~v~n" (expr-compare '(if x y z) '(g x y z)))
(printf "~v~n~n" "(if % (if x y z) (g x y z))")
; (printf "~v~n~n" (test-expr-compare '(if x y z) '(g x y z)))

; Custom test
(printf "~v~n" (expr-compare '(g x y z) '(if x y z)))
(printf "~v~n~n" "(if % (g x y z) (if x y z))")
; (printf "~v~n~n" (test-expr-compare '(g x y z) '(if x y z)))

(printf "~v~n" (expr-compare '((lambda (a) (f a)) 1) '((lambda (a) (g a)) 2)))
(printf "~v~n~n" "((lambda (a) ((if % f g) a)) (if % 1 2))")
; (printf "~v~n~n" (test-expr-compare '((lambda (a) (f a)) 1) '((lambda (a) (g a)) 2)))

(printf "~v~n" (expr-compare '((lambda (a) (f a)) 1) '((λ (a) (g a)) 2)))
(printf "~v~n~n" "((λ (a) ((if % f g) a)) (if % 1 2))")
; (printf "~v~n~n" (test-expr-compare '((lambda (a) (f a)) 1) '((λ (a) (g a)) 2)))

(printf "~v~n" (expr-compare '((lambda (a) a) c) '((lambda (b) b) d)))
(printf "~v~n" "((lambda (a!b) a!b) (if % c d))")
(printf "~v~n~n" (test-expr-compare '((lambda (a) a) c) '((lambda (b) b) d)))

(printf "~v~n" (expr-compare ''((λ (a) a) c) ''((lambda (b) b) d)))
(printf "~v~n" "(if % '((λ (a) a) c) '((lambda (b) b) d))")
(printf "~v~n~n" (test-expr-compare ''((λ (a) a) c) ''((lambda (b) b) d)))

(printf "~v~n" (expr-compare '(+ #f ((λ (a b) (f a b)) 1 2))
              '(+ #t ((lambda (a c) (f a c)) 1 2))))
(printf "~v~n~n" "(+ (not %) ((λ (a b!c) (f a b!c)) 1 2))")
; (printf "~v~n~n" (test-expr-compare '(+ #f ((λ (a b) (f a b)) 1 2))
;               '(+ #t ((lambda (a c) (f a c)) 1 2))))

(printf "~v~n" (expr-compare '((λ (a b) (f a b)) 1 2)
              '((λ (a b) (f b a)) 1 2)))
(printf "~v~n~n" "((λ (a b) (f (if % a b) (if % b a))) 1 2)")
; (printf "~v~n~n" (test-expr-compare '((λ (a b) (f a b)) 1 2)
;               '((λ (a b) (f b a)) 1 2)))

(printf "~v~n" (expr-compare '((λ (a b) (f a b)) 1 2)
              '((λ (a c) (f c a)) 1 2)))
(printf "~v~n~n" "((λ (a b!c) (f (if % a b!c) (if % b!c a))) 1 2)")
; (printf "~v~n~n" (test-expr-compare '((λ (a b) (f a b)) 1 2)
;               '((λ (a c) (f c a)) 1 2)))

(printf "~v~n" (expr-compare '((lambda (a) (eq? a ((λ (a b) ((λ (a b) (a b)) b a))
                                    a (lambda (a) a))))
                (lambda (b a) (b a)))
              '((λ (a) (eqv? a ((lambda (b a) ((lambda (a b) (a b)) b a))
                                a (λ (b) a))))
                (lambda (a b) (a b)))))
(printf "~v~n~n" "((λ (a) ((if % eq? eqv?) a ((λ (a!b b!a) ((λ (a b) (a b)) (if % b!a a!b) (if % a!b b!a))) a (λ (a!b) (if % a!b a))))) (lambda (b!a a!b) (b!a a!b)))")
; (printf "~v~n~n" (test-expr-compare '((lambda (a) (eq? a ((λ (a b) ((λ (a b) (a b)) b a))
;                                     a (lambda (a) a))))
;                 (lambda (b a) (b a)))
;               '((λ (a) (eqv? a ((lambda (b a) ((lambda (a b) (a b)) b a))
;                                 a (λ (b) a))))
;                 (lambda (a b) (a b)))))