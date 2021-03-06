#include "Mongoose.hpp"

#include <mvs/http/Tokeniser.hpp>
#include <mvs/http/Exception_instance.hpp>
#include <mvs/http/Stream_buf.hpp>



namespace http{

using namespace mg;

class RestServ : public mg::Mgr<RestServ>{
public:
    explicit RestServ(const char* webroot)
    {
        memset(&httpoptions_, 0x00, sizeof(httpoptions_));
        httpoptions_.document_root = webroot;
    }
    ~RestServ() noexcept {};

    // Copy.
    RestServ(const RestServ& rhs) = delete;
    RestServ& operator=(const RestServ& rhs) = delete;

    // Move.
    RestServ(RestServ&&) = delete;
    RestServ& operator=(RestServ&&) = delete;

    //reset
    void reset(mg::HttpMessage data) noexcept;
    void httpStatic(mg_connection& nc, HttpMessage data);
    void httpRequest(mg_connection& nc, HttpMessage data);

protected:
     std::pair<std::string_view,std::string_view> rpc_check(mg::HttpMessage &data){
        auto rpcuser = data.header("rpcuser");
        auto rpcpassword = data.header("rpcpassword");
    
        if (rpcuser.empty() || rpcpassword.empty()) {
       	    throw InvalidException{"rpc authorisation required"_sv};
        }
        return std::make_pair(rpcuser, rpcpassword);
     }

     void rpc_auth(std::string_view rpcuser, std::string_view rpcpassword) const {
        if( rpcuser_ == rpcuser and rpcpassword_ == rpcpassword) {
        } else {
            throw InvalidException("rpc authorisation failed"_sv);
        }
     }

private:

  enum : int {
    // Method values are represented as powers of two for simplicity.
    MethodGet = 1 << 0,
    MethodPost = 1 << 1,
    MethodPut = 1 << 2,
    MethodDelete = 1 << 3,
    // Method value mask.
    MethodMask = MethodGet | MethodPost | MethodPut | MethodDelete,

    // Subsequent bits represent matching components.
    MatchMethod = 1 << 4,
    MatchUri = 1 << 5,
    // Match result mask.
    MatchMask = MatchMethod | MatchUri
  };

    bool isSet(int bs) const noexcept { return (state_ & bs) == bs; }

    mg_serve_http_opts httpoptions_;
    std::string_view  rpcpassword_;
    std::string_view  rpcuser_;

    mg::OStream out_;
    Tokeniser<'/'> uri_;
    int state_{0};
    const char* const servername_{"Http-Metaverse"};
};

} // http
