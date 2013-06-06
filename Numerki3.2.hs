import Data.List (unfoldr)
bisect f (x0,x1) = let fx0 = f(x0) in let c = (x0+x1)/2 in let fxc = f(c) in if fx0*fxc < 0 then (x0,c) else (c,x1)

fx x = x * exp(-x) - 0.06064

alpha = 0.046926359947960

zadanie = 
	putStr $ map (\x -> if x==',' then ' ' else x) $
	  concatMap (\x -> if ((x==')') || (x=='(')) then [] else [x]) $ concatMap (\x -> (show x) ++ "\n") $
		map (\(n,(a,b)) -> (n,a,b, ((a+b/2) - alpha), (abs(b-a)/2))) $
			 unfoldr (\(n, interw) -> if n == 0 then Nothing else Just ((30-n,interw), (n-1, bisect fx interw))) (30, (0, 1))

main = zadanie
