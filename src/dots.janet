# Returns the coordinates and the radius for a bunch of random circles

(math/seedrandom	(os/time))

(for i 0 24
  (let [x (math/floor (* 640 (math/random)))
        y (math/floor (* 400 (math/random)))
        r (math/floor (* 40 (math/random)))]
    (printf "%d %d %d" x y r)))
