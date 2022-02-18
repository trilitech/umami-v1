let getComponent =
  fun
  | `google => <SVGGoogle />
  | `facebook => <SVGFacebook />
  | `discord => <SVGDiscord />
  | `reddit => <SVGReddit />
  | `github => <SVGGithub />
  | `twitter => <SVGTwitter />
  | `twitch => <SVGTwitch />;

let getIcon =
  fun
  | `google => Icons.Google.build
  | `facebook => Icons.Facebook.build
  | `discord => Icons.Discord.build
  | `reddit => Icons.Reddit.build
  | `github => Icons.Github.build
  | `twitter => Icons.Twitter.build
  | `twitch => Icons.Twitch.build;
