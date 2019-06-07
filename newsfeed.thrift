struct Profile{
    1: i32 id,
    2: string name,
    3: byte gender,
    4: i32 birthday
}

struct ProfileCreateResult{
    1: i32 exitCode,
    2: string message,
    3: i32 result
}

struct ProfileResult{
    1: i32 exitCode,
    2: string message,
    3: Profile  result
}

struct PostList {
    1: i32 id,
    2: i32 count,
    3: list<i32> listFeed
}

struct NewsFeed{
    1: i32 id,
    2: i32 owner,	
    3: string content,
    4: i32 edit_time, 
    5: byte status
}

struct FeedCreateResult{
    1: i32 exitCode,
    2: string message,
    3: i32 result
}

struct FeedUpdateResult{
    1: i32 exitCode,
    2: string message
}

struct FeedDeleteResult{
    1: i32 exitCode,
    2: string message
}

struct FeedResult{
    1: i32 exitCode,
    2: string message,
    3: NewsFeed result
}

struct Item{
    1:  i32 post,
    2:  i32 id
}

struct FeedCountResult{
    1: i32 exitCode,
    2: string message,
    3: Item result
}

struct ListFeed{
    1: i32 count,
    2: list<NewsFeed> feedlist,
    3: Item nex
}

struct ListFeedResult{
    1: i32 exitCode,
    2: string message,
    3: ListFeed result
}



service NewsFeedService {
    ProfileCreateResult createProfile( 1: string name, 2: byte gender,    3: i32 birthday),
    ProfileResult       getProfile(    1: i32 id),
    FeedCreateResult    createNewsFeed(1: i32 owner,   2: string content, 3: byte status),
    FeedUpdateResult    updateNewsFeed(1: i32 id,      2: string content, 3: byte status),
    FeedDeleteResult    deleteNewsFeed(1: i32 id,      2: i32 owner),
    FeedResult          getFeed(       1: i32 id),
    FeedCountResult     getFeedCount(  1: i32 owner),
    ListFeedResult      getListFeed(   1: i32 owner,   2: Item post,       3: i32 count),
}
