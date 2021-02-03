let isValidFloat = value => {
  let fieldState: ReSchema.fieldState =
    value->Js.Float.fromString->Js.Float.isNaN ? Error("not a float") : Valid;
  fieldState;
};

let isValidInt = value => {
  let fieldState: ReSchema.fieldState =
    value->Js.String2.length == 0 || value->int_of_string_opt->Option.isSome
      ? Valid : Error("not an int");
  fieldState;
};

let formFieldsAreValids = (fieldsState, validateFields) => {
  let fields = fieldsState->Array.map(((field, _)) => field);
  let fieldsState = validateFields(fields);

  fieldsState->Array.every((fieldState: ReForm.fieldState) => {
    switch (fieldState) {
    | Valid => true
    | _ => false
    }
  });
};
