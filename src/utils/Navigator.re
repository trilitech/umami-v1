module Clipboard = {
  let cb = [%raw "navigator.clipboard"];

  let write: string => unit = text => cb##writeText(text);
};
