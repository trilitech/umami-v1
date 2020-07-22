type state = {count: int};

type action =
  | Increment
  | Decrement;

let initialState = {count: 0};

let reducer = (state, action) =>
  switch (action) {
  | Increment => {count: state.count + 1}
  | Decrement => {count: state.count - 1}
  };

[@react.component]
let make = () => {
  let (state, dispatch) = React.useReducer(reducer, initialState);

  <main>
    {React.string("Counter")}
    <div>
      <button onClick={_ => dispatch(Decrement)}>
        {React.string("-")}
      </button>
      <span className="counter">
        {state.count |> string_of_int |> React.string}
      </span>
      <button onClick={_ => dispatch(Increment)}>
        {React.string("+")}
      </button>
    </div>
  </main>;
};
