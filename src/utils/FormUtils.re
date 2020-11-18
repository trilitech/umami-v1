let isValidFloat = value => {
  let fieldState: ReSchema.fieldState =
    value->Js.Float.fromString->Js.Float.isNaN ? Error("not a float") : Valid;
  fieldState;
};

let isValidInt = value => {
  let fieldState: ReSchema.fieldState =
    value->Js.String2.length == 0
    || value->int_of_string_opt->Belt.Option.isSome
      ? Valid : Error("not an int");
  fieldState;
};
