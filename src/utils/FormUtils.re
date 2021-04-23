let isValidFloat = value => {
  let fieldState: ReSchema.fieldState =
    value->Js.Float.fromString->Js.Float.isNaN
      ? Error(I18n.form_input_error#float) : Valid;
  fieldState;
};

let isValidTokenAmount = value => {
  let fieldState: ReSchema.fieldState =
    value->Js.String2.length == 0 || value->Token.Repr.isValid
      ? Valid : Error(I18n.form_input_error#int);
  fieldState;
};

let isValidInt = value => {
  let fieldState: ReSchema.fieldState =
    value->Js.String2.length == 0 || value->int_of_string_opt->Option.isSome
      ? Valid : Error(I18n.form_input_error#int);
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

let i18n = {
  ...ReSchemaI18n.default,
  stringNonEmpty: (~value as _) => I18n.form_input_error#string_empty,
};
