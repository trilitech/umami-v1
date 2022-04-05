include Belt.Array;

let replaceAtIndex = (arr, i, value) => {
  arr->mapWithIndex((j, oldValue) => j == i ? value : oldValue);
};

let removeAtIndex = (arr, i) => {
  arr->keepWithIndex((_, j) => i != j);
};
